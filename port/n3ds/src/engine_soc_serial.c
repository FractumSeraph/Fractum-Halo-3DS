/* A test of the 3DS's socket service as it is, in the Azahar emulator: the
 * service takes one call at a time from all of a program's threads (one
 * session), so a call that waits (a DNS lookup, a connect, a receive or a
 * poll that waits) holds up every other thread's socket calls. Azahar's
 * does not. With sdmc:/halo-source/soc-serial.txt present at start-up,
 * every socket function libctru has is called under one lock here (the
 * linker's --wrap: port/n3ds/build.py), which holds the others up as the
 * console's service would; a call that holds it more than a second is
 * logged. Without the file the functions are called as they are. */
#include <3ds.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

int n3ds_soc_serial;
static RecursiveLock serial_lock;
static int serial_ready;
void n3ds_log(const char *message);

void n3ds_soc_serial_start(void)
{
    FILE *flag=fopen("sdmc:/halo-source/soc-serial.txt","r");
    if(!flag) return;
    fclose(flag);
    RecursiveLock_Init(&serial_lock);serial_ready=1;n3ds_soc_serial=1;
    n3ds_log("SOC SERIAL: socket calls made one at a time, as the console's service takes them (soc-serial.txt)");
}

static u64 serial_enter(void)
{
    if(!n3ds_soc_serial) return 0;
    RecursiveLock_Lock(&serial_lock);
    return svcGetSystemTick();
}
static void serial_leave(u64 start,const char *what)
{
    if(!n3ds_soc_serial) return;
    u64 held=svcGetSystemTick()-start;
    RecursiveLock_Unlock(&serial_lock);
    if(held>SYSCLOCK_ARM11) {
        char text[96];snprintf(text,sizeof(text),"SOC SERIAL: %s held the socket service %llu ms",what,
            (unsigned long long)(held/(SYSCLOCK_ARM11/1000)));
        n3ds_log(text);
    }
}

/* port: the service closed under the game's threads no more. At the
 * game's end (engine_main.c) a thread still in the network's loop called
 * poll after socExit, which read the socket table libctru had let go of
 * (null: a stop on the console, as the Azahar emulator showed). Once
 * n3ds_soc_close begins, every socket call fails (ENETDOWN), and socExit
 * waits for the calls already in the service to come back */
static volatile int soc_closed,soc_calls;
static int soc_enter(void)
{
    __atomic_add_fetch(&soc_calls,1,__ATOMIC_SEQ_CST);
    if(!__atomic_load_n(&soc_closed,__ATOMIC_SEQ_CST)) return 1;
    __atomic_sub_fetch(&soc_calls,1,__ATOMIC_SEQ_CST);
    errno=ENETDOWN;
    return 0;
}
static void soc_leave(void) {__atomic_sub_fetch(&soc_calls,1,__ATOMIC_SEQ_CST);}
void n3ds_soc_close(void)
{
    __atomic_store_n(&soc_closed,1,__ATOMIC_SEQ_CST);
    /* (a call that waits is a poll of none, 4 ms at most: posix_3ds_poll;
     * a DNS lookup or connect may take longer, two seconds given them) */
    for(int waited=0;__atomic_load_n(&soc_calls,__ATOMIC_SEQ_CST) && waited<2000;++waited) svcSleepThread(1000000LL);
    socExit();
}

#define WRAP_FAILING(type,name,params,args,failed) \
    type __real_##name params; \
    type __wrap_##name params { if(!soc_enter()) return failed; u64 serial_start_=serial_enter(); type serial_result_=__real_##name args; serial_leave(serial_start_,#name); soc_leave(); return serial_result_; }
#define WRAP(type,name,params,args) WRAP_FAILING(type,name,params,args,(type)-1)

WRAP(int,socket,(int d,int t,int p),(d,t,p))
WRAP(int,bind,(int s,const struct sockaddr *a,socklen_t l),(s,a,l))
WRAP(int,connect,(int s,const struct sockaddr *a,socklen_t l),(s,a,l))
WRAP(int,listen,(int s,int b),(s,b))
WRAP(int,accept,(int s,struct sockaddr *a,socklen_t *l),(s,a,l))
WRAP(ssize_t,recv,(int s,void *b,size_t n,int f),(s,b,n,f))
WRAP(ssize_t,recvfrom,(int s,void *b,size_t n,int f,struct sockaddr *a,socklen_t *l),(s,b,n,f,a,l))
WRAP(ssize_t,send,(int s,const void *b,size_t n,int f),(s,b,n,f))
WRAP(ssize_t,sendto,(int s,const void *b,size_t n,int f,const struct sockaddr *a,socklen_t l),(s,b,n,f,a,l))
WRAP(int,poll,(struct pollfd *f,nfds_t n,int t),(f,n,t))
WRAP(int,select,(int n,fd_set *r,fd_set *w,fd_set *e,struct timeval *t),(n,r,w,e,t))
WRAP(int,getaddrinfo,(const char *n,const char *s,const struct addrinfo *h,struct addrinfo **r),(n,s,h,r))
WRAP_FAILING(struct hostent *,gethostbyname,(const char *n),(n),NULL)
WRAP_FAILING(long,gethostid,(void),(),0)
WRAP(int,setsockopt,(int s,int l,int o,const void *v,socklen_t n),(s,l,o,v,n))
WRAP(int,getsockopt,(int s,int l,int o,void *v,socklen_t *n),(s,l,o,v,n))
WRAP(int,getsockname,(int s,struct sockaddr *a,socklen_t *l),(s,a,l))
WRAP(int,getpeername,(int s,struct sockaddr *a,socklen_t *l),(s,a,l))
WRAP(int,shutdown,(int s,int h),(s,h))
