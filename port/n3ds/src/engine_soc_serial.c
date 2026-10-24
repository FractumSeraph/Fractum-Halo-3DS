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
#include <netinet/in.h>

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
    /* (a call still in the service after that: the service is left as it
     * is, its buffer with it, rather than freed under the thread; the
     * system takes it back with the program) */
    if(__atomic_load_n(&soc_calls,__ATOMIC_SEQ_CST)) {n3ds_log("NETWORK: a socket call still in the service at exit; the service left open");return;}
    socExit();
}

#define WRAP_FAILING(type,name,params,args,failed) \
    type __real_##name params; \
    type __wrap_##name params { if(!soc_enter()) return failed; u64 serial_start_=serial_enter(); type serial_result_=__real_##name args; serial_leave(serial_start_,#name); soc_leave(); return serial_result_; }
#define WRAP(type,name,params,args) WRAP_FAILING(type,name,params,args,(type)-1)

static unsigned char soc_fds[1024/8];
/* (and which are bound, by a bind, a connect or a send, and which connected:
 * a getsockname of a socket not bound, or a getpeername of one not connected,
 * stopped the Azahar emulator, CTRSockAddr::FromPlatform; the console's
 * service answers EINVAL and ENOTCONN, as these do) */
static unsigned char soc_bound[1024/8],soc_connected[1024/8];
#define SOC_FD_SET(set,fd) do { if((fd)>=0 && (fd)<1024) (set)[(fd)/8]|=(unsigned char)(1<<((fd)%8)); } while(0)
#define SOC_FD_CLEAR(set,fd) do { if((fd)>=0 && (fd)<1024) (set)[(fd)/8]&=(unsigned char)~(1<<((fd)%8)); } while(0)
#define SOC_FD_TEST(set,fd) ((fd)<0 || (fd)>=1024 || (((set)[(fd)/8]>>((fd)%8))&1))

/* port: an address not IPv4 refused before the service sees it (EAFNOSUPPORT,
 * as the console's service answers): the Azahar emulator's stops on one
 * (CTRSockAddr::ToPlatform). The first few logged with their caller */
static int address_refused(const struct sockaddr *a,socklen_t l,const char *what,void *caller)
{
    if(!a || (l>=(socklen_t)sizeof(struct sockaddr_in) && a->sa_family==AF_INET)) return 0;
    static int logged;
    if(logged<6) {
        logged++;
        char text[128];snprintf(text,sizeof(text),"SOC: %s refused an address of family %d, length %d (caller %p)",
            what,(int)a->sa_family,(int)l,caller);
        n3ds_log(text);
    }
    errno=EAFNOSUPPORT;
    return 1;
}
int __real_bind(int s,const struct sockaddr *a,socklen_t l);
int __wrap_bind(int s,const struct sockaddr *a,socklen_t l)
{
    if(address_refused(a,l,"bind",__builtin_return_address(0)) || !soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_bind(s,a,l); serial_leave(serial_start_,"bind"); soc_leave();
    if(!r) SOC_FD_SET(soc_bound,s);
    return r;
}
int __real_connect(int s,const struct sockaddr *a,socklen_t l);
int __wrap_connect(int s,const struct sockaddr *a,socklen_t l)
{
    if(address_refused(a,l,"connect",__builtin_return_address(0)) || !soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_connect(s,a,l); serial_leave(serial_start_,"connect"); soc_leave();
    /* (a connect under way binds too; connected once it is made: a
     * datagram socket's at once, a stream's by the time it is asked, the
     * port's callers asking only after the connect is done) */
    if(!r || errno==EINPROGRESS || errno==EALREADY || errno==EAGAIN) {SOC_FD_SET(soc_bound,s);SOC_FD_SET(soc_connected,s);}
    return r;
}
ssize_t __real_sendto(int s,const void *b,size_t n,int f,const struct sockaddr *a,socklen_t l);
ssize_t __wrap_sendto(int s,const void *b,size_t n,int f,const struct sockaddr *a,socklen_t l)
{
    if(address_refused(a,l,"sendto",__builtin_return_address(0)) || !soc_enter()) return -1;
    u64 serial_start_=serial_enter(); ssize_t r=__real_sendto(s,b,n,f,a,l); serial_leave(serial_start_,"sendto"); soc_leave();
    if(r>=0) SOC_FD_SET(soc_bound,s);
    return r;
}

/* port: the sockets, by descriptor: a close of one after the service has
 * closed (n3ds_soc_close, at the game's end, while the internet play thread
 * still runs) is no call: the C library's close looked the device up in a
 * table socExit had emptied (a null read: the crash on leaving the game
 * from the HOME menu) */
static void soc_odd_call(const char *what,int fd,void *caller)
{
    static int logged;
    if(logged<6) {
        logged++;
        char text[128];snprintf(text,sizeof(text),"SOC: %s of socket %d, which is not %s (caller %p)",what,fd,
            what[3]=='p'?"connected":"bound",caller);
        n3ds_log(text);
    }
}
int __real_getsockname(int s,struct sockaddr *a,socklen_t *l);
int __wrap_getsockname(int s,struct sockaddr *a,socklen_t *l)
{
    if(!SOC_FD_TEST(soc_bound,s)) {soc_odd_call("getsockname",s,__builtin_return_address(0));errno=EINVAL;return -1;}
    if(!soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_getsockname(s,a,l); serial_leave(serial_start_,"getsockname"); soc_leave();
    return r;
}
int __real_getpeername(int s,struct sockaddr *a,socklen_t *l);
int __wrap_getpeername(int s,struct sockaddr *a,socklen_t *l)
{
    if(!SOC_FD_TEST(soc_connected,s)) {soc_odd_call("getpeername",s,__builtin_return_address(0));errno=ENOTCONN;return -1;}
    if(!soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_getpeername(s,a,l); serial_leave(serial_start_,"getpeername"); soc_leave();
    return r;
}
static void soc_fd_made(int fd) {if(fd>=0 && fd<1024) {soc_fds[fd/8]|=(unsigned char)(1<<(fd%8));soc_bound[fd/8]&=(unsigned char)~(1<<(fd%8));soc_connected[fd/8]&=(unsigned char)~(1<<(fd%8));}}
int __real_socket(int d,int t,int p);
int __wrap_socket(int d,int t,int p)
{
    if(!soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_socket(d,t,p); serial_leave(serial_start_,"socket"); soc_leave();
    soc_fd_made(r); return r;
}
int __real_accept(int s,struct sockaddr *a,socklen_t *l);
int __wrap_accept(int s,struct sockaddr *a,socklen_t *l)
{
    if(!soc_enter()) return -1;
    u64 serial_start_=serial_enter(); int r=__real_accept(s,a,l); serial_leave(serial_start_,"accept"); soc_leave();
    soc_fd_made(r); SOC_FD_SET(soc_bound,r); SOC_FD_SET(soc_connected,r); return r;
}
int __real_close(int fd);
int __wrap_close(int fd)
{
    if(fd<0 || fd>=1024 || !((soc_fds[fd/8]>>(fd%8))&1)) return __real_close(fd);
    soc_fds[fd/8]&=(unsigned char)~(1<<(fd%8));
    SOC_FD_CLEAR(soc_bound,fd);SOC_FD_CLEAR(soc_connected,fd);
    if(!soc_enter()) return 0;
    u64 serial_start_=serial_enter(); int r=__real_close(fd); serial_leave(serial_start_,"close"); soc_leave();
    return r;
}
WRAP(int,listen,(int s,int b),(s,b))
WRAP(ssize_t,recv,(int s,void *b,size_t n,int f),(s,b,n,f))
WRAP(ssize_t,recvfrom,(int s,void *b,size_t n,int f,struct sockaddr *a,socklen_t *l),(s,b,n,f,a,l))
WRAP(ssize_t,send,(int s,const void *b,size_t n,int f),(s,b,n,f))
WRAP(int,poll,(struct pollfd *f,nfds_t n,int t),(f,n,t))
WRAP(int,select,(int n,fd_set *r,fd_set *w,fd_set *e,struct timeval *t),(n,r,w,e,t))
WRAP(int,getaddrinfo,(const char *n,const char *s,const struct addrinfo *h,struct addrinfo **r),(n,s,h,r))
WRAP_FAILING(struct hostent *,gethostbyname,(const char *n),(n),NULL)
WRAP_FAILING(long,gethostid,(void),(),0)
WRAP(int,setsockopt,(int s,int l,int o,const void *v,socklen_t n),(s,l,o,v,n))
WRAP(int,getsockopt,(int s,int l,int o,void *v,socklen_t *n),(s,l,o,v,n))
WRAP(int,shutdown,(int s,int h),(s,h))
