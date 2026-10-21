#include <3ds.h>
#include <citro3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "engine_textures.h"
#include "engine_renderer.h"
#include "engine_parallel.h"

static FILE *log_file;
static char log_buffer[16*1024];
static int graphics_owned;
static int error_console;
static int loading_console=1;
/* port: every line written to the SD card as it comes, not queued, when
 * sdmc:/halo-source/sync-log.txt exists: a console's crash (a data abort,
 * which ends the game at once) then loses none of the log's last lines.
 * Slower; for finding a crash only. */
static int log_sync;
/* port: the last lines logged, for the stall watcher to write out when the
 * game stops making progress (engine_platform.c): which a hang, at start-up
 * or loading a map, says nothing else of. Lines may tear when two threads
 * log at once; it is a diagnostic. */
enum { RECENT_LINES=24, RECENT_LENGTH=160 };
static char recent[RECENT_LINES][RECENT_LENGTH];
static volatile unsigned int recent_next;
volatile unsigned int n3ds_log_progress_tick;
static void recent_add(const char *message)
{
    unsigned int slot=__sync_fetch_and_add(&recent_next,1)%RECENT_LINES;
    snprintf(recent[slot],RECENT_LENGTH,"%s",message);
    n3ds_log_progress_tick=(unsigned int)svcGetSystemTick();
}
/* the recent lines, oldest first, written to file */
void n3ds_log_recent_write(FILE *file)
{
    unsigned int next=recent_next;
    for(unsigned int i=0;i<RECENT_LINES;++i) {
        const char *line=recent[(next+i)%RECENT_LINES];
        if(line[0]) fprintf(file,"  | %.*s\n",RECENT_LENGTH,line);
    }
}
#include "engine_log_queue.inl"
void n3ds_log_shutdown(void) { log_async_finish(); }
/* libctru applies this during startup: New 3DS 804 MHz CPU and L2 cache. */
bool __ctru_speedup = true;
#ifdef HALO_N3DS_LOW_MEMORY_TEST
/* Explicit developer fixture: approximate the original 64 MiB process after
 * executable/stack overhead, without borrowing Azahar's extra app memory. */
u32 __ctru_heap_size = 24U*1024*1024;
u32 __ctru_linear_heap_size = 32U*1024*1024;
#endif
int halo_engine_boot(void);
static void flush_console(void)
{
    if(!error_console && n3ds_gpu_bottom_owned()) graphics_owned=1;
    if(graphics_owned) return;
    u16 width, height;
    u8 *buffer=gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &width, &height);
    GSPGPU_FlushDataCache(buffer, width*height*gspGetBytesPerPixel(gfxGetScreenFormat(GFX_BOTTOM)));
    gfxScreenSwapBuffers(GFX_BOTTOM, false);
}
void n3ds_log(const char *message)
{
    /* Keep the original failure even if the next launch replaces its session
     * log. Fatal paths are rare; close immediately to commit to the SD card. */
    if(strstr(message,"FATAL:") || strstr(message,"FAIL:") || !strcmp(message,"RESULT: FAIL")) {
        FILE *fatal=fopen("sdmc:/halo-source-fatal.log","a");
        if(fatal) {fprintf(fatal,"[%lld] %s\n",(long long)time(NULL),message);fclose(fatal);}
    }
    recent_add(message);
    if(!error_console && n3ds_gpu_bottom_owned()) graphics_owned=1;
    if(!graphics_owned && (!loading_console || error_console)) {printf("%s\n", message);fflush(stdout);}
    if(log_sync) {
        if(log_file) {fprintf(log_file,"%s\n",message);fflush(log_file);}
        flush_console();return;
    }
    if(graphics_owned && !log_thread) log_async_start();
    if(log_thread) log_enqueue(message);
    else if (log_file) {
        fprintf(log_file, "%s\n", message);
        /* Batch routine diagnostics between the existing 30-frame heartbeat.
         * Flush startup, errors and final results immediately. This removes
         * dozens of SD transactions from each diagnostic burst while keeping
         * the wall-clock test observer's frame markers promptly visible. */
        if(!graphics_owned || !strncmp(message,"STARTUP:",8) ||
           !strncmp(message,"NATIVE MEMORY:",14) || !strncmp(message,"FRONTEND FRAME:",15) ||
           !strncmp(message,"MISSION TICK END:",17) ||
           !strncmp(message,"RESULT:",7) || !strncmp(message,"EXIT:",5) || strstr(message,"FATAL:") ||
           strstr(message,"FAIL") || strstr(message,"ERROR") || strstr(message,"ASSERT"))
            fflush(log_file);
    }
    flush_console();
}
/* port: a stop the game cannot go on from (an assert: runtime.c's
 * system_exit after a FATAL): said on the bottom screen, the sound stopped,
 * the log written, until START. Before, the game left at once, the player
 * seeing nothing (and its other threads, still running as the services were
 * closed, could take the console down with it) */
void n3ds_fatal_screen(const char *what)
{
    static volatile int shown;
    if(shown++) for(;;) svcSleepThread(1000000000LL);
    ndspSetMasterVol(0.f);
    error_console=1;graphics_owned=0;gfxSet3D(false);gfxSetDoubleBuffering(GFX_BOTTOM,false);
    consoleInit(GFX_BOTTOM,NULL);
    printf("Fractum Halo stopped.\n\n%.300s\n\nThe last lines are in\nhalo-source-engine.log and\nhalo-source-fatal.log on the SD card.\n\nPress START to exit.\n",what?what:"");
    fflush(stdout);flush_console();
    if(log_thread) log_async_finish();
    if(log_file) fflush(log_file);
    while(aptMainLoop()) {
        hidScanInput();
        if(hidKeysDown()&KEY_START) break;
        flush_console();gspWaitForVBlank();
    }
}
#include "engine_log_tests.inl"
int main(void)
{
    gfxInitDefault(); gfxSetDoubleBuffering(GFX_BOTTOM, false); consoleInit(GFX_BOTTOM, NULL);
    printf("\n\n           Fractum Halo\n\n\n          Loading game data...\n\n    Initial loading may take a while.\n");
    fflush(stdout);flush_console();
    remove("sdmc:/halo-source-engine.previous.log");
    rename("sdmc:/halo-source-engine.log","sdmc:/halo-source-engine.previous.log");
    log_file = fopen("sdmc:/halo-source-engine.log", "w");
    if(log_file) setvbuf(log_file,log_buffer,_IOFBF,sizeof(log_buffer));
    {FILE *flag=fopen("sdmc:/halo-source/sync-log.txt","r");if(flag) {fclose(flag);log_sync=1;}}
    n3ds_log("Halo native engine bootstrap");
    n3ds_log("BUILD: Halo3DS Original-model adaptive rendering 2026-10-05");
    if(log_sync) n3ds_log("LOG: each line written as it comes (halo-source/sync-log.txt)");
#ifdef HALO_N3DS_LOW_MEMORY_TEST
    n3ds_log("LOW MEMORY FIXTURE: 24 MiB application heap + 32 MiB linear heap");
#endif
    bool new_model=false;APT_CheckNew3DS(&new_model);
    n3ds_log(new_model?"MODEL: New 3DS family; high-speed CPU and L2 cache requested":"MODEL: original 3DS/2DS family; standard CPU speed");
    /* Retain capacity for two A10 corridor views and their effects, while
     * keeping every draw and the live command-capacity checks. */
    n3ds_parallel_initialize();
    /* Halo PC maps (Custom Edition's) need the New 3DS's memory: their tag
     * cache's space and its conversion take about 40 MB (engine_ce_cache.inl),
     * which leaves room only with 96 MB or more for the game (what the
     * Homebrew Launcher's title gives it). Otherwise the menus say why. */
    {
        extern int platform_ce_tag_cache_ready;
        extern char platform_ce_tag_cache_error[160];
        unsigned int application=(unsigned int)osGetMemRegionSize(MEMREGION_APPLICATION);
        char text[160];
        if (new_model && application>=96u*1024*1024) {platform_ce_tag_cache_ready=1;platform_ce_tag_cache_error[0]=0;}
        else if (new_model) snprintf(platform_ce_tag_cache_error,160,
            "they need 96 MB of memory for the game, and it was given %u MB",application>>20);
        snprintf(text,sizeof(text),"CE MAPS: %s (application memory %u MB)",
            platform_ce_tag_cache_ready?"playable":platform_ce_tag_cache_error,application>>20);
        n3ds_log(text);
    }
    /* The console's socket service, for system link and internet play (the
     * fork's Winsock layer, port/linux/src/xnet.c). Without it the game runs
     * offline: the transport reports no network. */
    static u32 *soc_buffer;
    soc_buffer = memalign(0x1000, 0x100000);
    Result soc_result = soc_buffer ? socInit(soc_buffer, 0x100000) : -1;
    if (R_FAILED(soc_result)) {
        char text[96];snprintf(text,sizeof(text),"NETWORK: socket service unavailable (%08lx); offline",(unsigned long)soc_result);
        n3ds_log(text);free(soc_buffer);soc_buffer=NULL;
    } else n3ds_log("NETWORK: socket service started (1 MiB buffer)");
    {extern void n3ds_soc_serial_start(void);n3ds_soc_serial_start();}
    int gpu_ready = C3D_Init(8 * C3D_DEFAULT_CMDBUF_SIZE);
    if (gpu_ready) n3ds_log("GPU command buffer: 2097152 bytes (two-eye capacity)");
    int programs_ready = gpu_ready && n3ds_gpu_programs_initialize();
    int textures_ready = programs_ready && n3ds_gpu_textures_initialize();
    extern void n3ds_stall_watch_start(void),n3ds_stall_watch_stop(void);
    n3ds_stall_watch_start();
    int result = textures_ready ? halo_engine_boot() : 1;
    extern void n3ds_prefetch_shutdown(void);
    n3ds_prefetch_shutdown();
    n3ds_stall_watch_stop();
    if (textures_ready) n3ds_gpu_textures_dispose();
    if (gpu_ready) C3D_Fini();
    if (programs_ready) n3ds_gpu_programs_dispose();
    if(result) {
        /* Render targets may have taken over the bottom screen. Reclaim it
         * after GPU shutdown so a startup failure cannot look like a freeze. */
        error_console=1;graphics_owned=0;gfxSet3D(false);gfxSetDoubleBuffering(GFX_BOTTOM,false);
        consoleInit(GFX_BOTTOM,NULL);
        printf("Fractum Halo stopped during startup.\n\nSee halo-source-engine.log\non the SD card for details.\n\nPress START to exit.\n");
        fflush(stdout);flush_console();
    }
    n3ds_log(result ? "RESULT: FAIL" : "RESULT: PASS");
    /* port: (the service closed once no thread is in it: engine_soc_serial.c) */
    if (soc_buffer) {extern void n3ds_soc_close(void);n3ds_soc_close();free(soc_buffer);soc_buffer=NULL;}
    n3ds_parallel_shutdown();log_async_finish();
    if (log_file) {fclose(log_file);log_file=NULL;}
    while (result && aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
        flush_console(); gspWaitForVBlank();
    }
    gfxExit(); return result;
}
