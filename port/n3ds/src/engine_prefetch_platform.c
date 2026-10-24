/* Speculative read-only scenery I/O. No engine pointers, stdio, allocation or
 * GPU operations on the worker. The owner retains the destination until the
 * captured kernel thread handle signals completion. */
#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
static Thread reader;
static Handle reader_thread,reader_file;
static LightEvent start,wake;
static unsigned char *destination;
static u32 read_offset,read_bytes;
static int cancel,urgent,result,exit_registered;
void n3ds_prefetch_shutdown(void);
static void read_worker(void *unused)
{
    (void)unused;LightEvent_Wait(&start);
    /* Let initial scene/model/sound requests finish first. Wake immediately if
     * the section is actually requested or the map is being unloaded. */
    LightEvent_WaitTimeout(&wake,2000000000LL);
    u32 done=0;
    while(done<read_bytes && !__atomic_load_n(&cancel,__ATOMIC_ACQUIRE)) {
        u32 bytes=read_bytes-done,got=0;if(bytes>65536) bytes=65536;
        if(R_FAILED(FSFILE_Read(reader_file,&got,(u64)read_offset+done,destination+done,bytes)) || !got || got>bytes) break;
        done+=got;
        if(!__atomic_load_n(&urgent,__ATOMIC_ACQUIRE)) LightEvent_WaitTimeout(&wake,2000000LL);
    }
    result=done==read_bytes;
}
int n3ds_prefetch_begin(const char *path,void *data,unsigned int offset,unsigned int bytes)
{
    if(reader || !path || strncmp(path,"sdmc:/",6) || !data || !bytes) return 0;
    if(R_FAILED(FSUSER_OpenFileDirectly(&reader_file,ARCHIVE_SDMC,fsMakePath(PATH_EMPTY,NULL),fsMakePath(PATH_ASCII,path+5),FS_OPEN_READ,0))) return 0;
    u64 size=0;
    if(R_FAILED(FSFILE_GetSize(reader_file,&size)) || offset>size || bytes>size-offset) {FSFILE_Close(reader_file);reader_file=0;return 0;}
    destination=data;read_offset=offset;read_bytes=bytes;cancel=urgent=result=0;
    LightEvent_Init(&start,RESET_ONESHOT);LightEvent_Init(&wake,RESET_ONESHOT);
    s32 priority=0x30;svcGetThreadPriority(&priority,CUR_THREAD_HANDLE);
    reader=threadCreate(read_worker,NULL,16384,priority<0x3d?priority+2:priority,-2,false);
    if(!reader) {FSFILE_Close(reader_file);reader_file=0;destination=NULL;return 0;}
    reader_thread=threadGetHandle(reader);
    if(!exit_registered) {atexit(n3ds_prefetch_shutdown);exit_registered=1;}
    LightEvent_Signal(&start);return 1;
}
/* 0 = still running, 1 = full read, -1 = failed/cancelled. */
int n3ds_prefetch_collect(int wait)
{
    if(!reader) return -1;
    if(wait) {__atomic_store_n(&urgent,1,__ATOMIC_RELEASE);LightEvent_Signal(&wake);}
    Result ready=svcWaitSynchronization(reader_thread,wait?U64_MAX:0);
    if(ready!=0) return 0;
    int success=result;threadFree(reader);reader=NULL;reader_thread=0;
    FSFILE_Close(reader_file);reader_file=0;destination=NULL;
    return success?1:-1;
}
void n3ds_prefetch_shutdown(void)
{
    if(!reader) return;
    __atomic_store_n(&cancel,1,__ATOMIC_RELEASE);LightEvent_Signal(&wake);
    if(svcWaitSynchronization(reader_thread,U64_MAX)!=0) svcBreak(USERBREAK_PANIC);
    threadFree(reader);reader=NULL;reader_thread=0;FSFILE_Close(reader_file);reader_file=0;destination=NULL;
}
int n3ds_prefetch_tests(void)
{
    static unsigned char actual[65539+32],expected[65539];
    const char *path="sdmc:/halo-source/ui.map";
    FILE *file=fopen(path,"rb");if(!file) return 0;
    int good=!fseek(file,4099,SEEK_SET) && fread(expected,1,sizeof(expected),file)==sizeof(expected);
    fclose(file);if(!good) return 0;
    for(unsigned int i=0;i<4;++i) {
        memset(actual,0x73,sizeof(actual));
        if(!n3ds_prefetch_begin(path,actual+16,4099,sizeof(expected))) return 0;
        if(n3ds_prefetch_collect(1)!=1 || memcmp(actual+16,expected,sizeof(expected))) return 0;
        for(unsigned int j=0;j<16;++j) if(actual[j]!=0x73 || actual[16+sizeof(expected)+j]!=0x73) return 0;
        if(!n3ds_prefetch_begin(path,actual+16,4099,sizeof(expected))) return 0;
        n3ds_prefetch_shutdown(); /* cancel during startup/read; join before reuse */
    }
    if(n3ds_prefetch_begin(path,actual,0xfffffff0u,64) || n3ds_prefetch_begin("invalid",actual,0,32)) return 0;
    extern void n3ds_log(const char *);
    n3ds_log("PASS: scenery prefetch: four exact cross-chunk reads, guards, cancel/restart and invalid ranges");return 1;
}

/* port: a whole file read into memory by a thread of its own, below the game
 * thread: a map's texture archive (.ntx, engine_texture_archive.inl), so that
 * the textures a match first draws come from memory and not the SD card (a
 * large card's reads were the render's hitches, 50-600 ms, on the console).
 * The buffer is the caller's to read once the state is 2; until then it falls
 * back to the file. One at a time; n3ds_slurp_end cancels, joins and frees */
static Thread slurp_thread;
static Handle slurp_handle,slurp_file;
static unsigned char *slurp_buffer;
static u32 slurp_size;
static volatile int slurp_state,slurp_cancel,slurp_exit_registered,slurp_owned;
static u64 slurp_started,slurp_ticks;
void n3ds_slurp_end(void);
static void slurp_worker(void *unused)
{
    (void)unused;
    u32 done=0;
    while(done<slurp_size && !__atomic_load_n(&slurp_cancel,__ATOMIC_ACQUIRE)) {
        u32 bytes=slurp_size-done,got=0;if(bytes>262144) bytes=262144;
        if(R_FAILED(FSFILE_Read(slurp_file,&got,(u64)done,slurp_buffer+done,bytes)) || !got || got>bytes) break;
        done+=got;
    }
    slurp_ticks=svcGetSystemTick()-slurp_started;
    __atomic_store_n(&slurp_state,done==slurp_size?2:-1,__ATOMIC_RELEASE);
}
void *n3ds_slurp_begin(const char *path,unsigned int size,void *into)
{
    if(slurp_thread || !path || strncmp(path,"sdmc:/",6) || !size) return NULL;
    if(R_FAILED(FSUSER_OpenFileDirectly(&slurp_file,ARCHIVE_SDMC,fsMakePath(PATH_EMPTY,NULL),fsMakePath(PATH_ASCII,path+5),FS_OPEN_READ,0))) return NULL;
    u64 file_size=0;
    /* (into a buffer of the caller's when given: the Custom Edition maps'
     * space, kept for the run, so no allocation of its own fragments the heap) */
    slurp_owned=!into;
    if(R_FAILED(FSFILE_GetSize(slurp_file,&file_size)) || file_size!=size || !(slurp_buffer=into?into:malloc(size))) {
        FSFILE_Close(slurp_file);slurp_file=0;return NULL;
    }
    slurp_size=size;slurp_cancel=0;slurp_state=1;slurp_started=svcGetSystemTick();
    s32 priority=0x30;svcGetThreadPriority(&priority,CUR_THREAD_HANDLE);
    slurp_thread=threadCreate(slurp_worker,NULL,16384,priority<0x3d?priority+2:priority,-2,false);
    if(!slurp_thread) {if(slurp_owned) free(slurp_buffer);slurp_buffer=NULL;FSFILE_Close(slurp_file);slurp_file=0;slurp_state=0;return NULL;}
    slurp_handle=threadGetHandle(slurp_thread);
    if(!slurp_exit_registered) {atexit(n3ds_slurp_end);slurp_exit_registered=1;}
    return slurp_buffer;
}
/* 0 none, 1 reading, 2 all in memory, -1 failed (the file to be read instead) */
int n3ds_slurp_state(void) { return __atomic_load_n(&slurp_state,__ATOMIC_ACQUIRE); }
unsigned int n3ds_slurp_milliseconds(void) { return (unsigned int)(slurp_ticks/(SYSCLOCK_ARM11/1000)); }
void n3ds_slurp_end(void)
{
    if(!slurp_thread) return;
    __atomic_store_n(&slurp_cancel,1,__ATOMIC_RELEASE);
    if(svcWaitSynchronization(slurp_handle,U64_MAX)!=0) svcBreak(USERBREAK_PANIC);
    threadFree(slurp_thread);slurp_thread=NULL;slurp_handle=0;
    FSFILE_Close(slurp_file);slurp_file=0;
    if(slurp_owned) free(slurp_buffer);
    slurp_buffer=NULL;slurp_size=0;slurp_state=0;slurp_owned=0;
}
