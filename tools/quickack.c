/*
 * Override socket syscalls using LD_PRELOAD to set the TCP_QUICKACK setsockopt.
 * Initially I tried setting TCP_QUICKACK by overriding socket connect() call
 * (before and after succesfull connect()), but this didn't seem to work. After
 * that I overrided socket recv() syscall and tried setting TCP_QUICKACK there.
 * Either case, I could not get TCP_QUICKACK to work. I then debugged the
 * TCP_QUICKACK handling using ebpf and found that quick ack support seems to
 * be broken. I tried with ver 5.3.0.
 *
 */
#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int g_interested_fd = -1;

int set_quickack(void)
{
    socklen_t val = 0;
    if (g_interested_fd < 0) {
        return 0;
    }
    if (getsockopt(g_interested_fd, IPPROTO_TCP, TCP_QUICKACK, (void *)&val, &val)) {
        printf("FAILED SETTING QUICKACK\n");
        return -1;
    }
    if (val > 0) {
        return 0;
    }

    val = 1;
    if(setsockopt(g_interested_fd, IPPROTO_TCP, TCP_QUICKACK, (void *)&val, sizeof(val))) {
        printf("FAILED SETTING QUICKACK\n");
        return -1;
    }
#if 0
    val = 0;
    if (getsockopt(g_interested_fd, IPPROTO_TCP, TCP_QUICKACK, (void *)&val, &val)) {
        printf("FAILED SETTING QUICKACK\n");
        return -1;
    }
    if (val <= 0) {
        printf("CUD NOT RETRIEVE QUICKACK\n");
        return 0;
    }
#endif

    printf("QUICKACK SET ok fd=%d\n", g_interested_fd);
    return 0;
}

void *get_func(char *apiname)
{
    void *func = dlsym(RTLD_NEXT, apiname);
    if(!func) {
        printf("we are funced, cudnot load original %s!\n", apiname);
        exit(1);
    }
    return func;
}

typedef int (*socket_func)(int domain, int type, int protocol);
int socket(int domain, int type, int protocol)
{
    int ret;
    void *func = get_func("socket");

    ret = ((socket_func)func)(domain, type, protocol);
    if ((SOCK_STREAM == type) && (ret >= 0)) {
        printf("set interested fd=%d\n", ret);
        g_interested_fd = ret;
        set_quickack();
    }
    return ret;
}

typedef int (*connect_func)(int, const struct sockaddr *, socklen_t); 
int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    void *func = get_func("connect");
    ret = ((connect_func)func)(fd, addr, addrlen);
    set_quickack();
    return ret;
}

#if 0
typedef ssize_t (*recv_func)(int, void *, size_t, int);
ssize_t recv(int fd, void *buf, size_t len, int flags)
{
    void *func = get_func("recv");
    set_quickack();
    return ((recv_func)func)(fd, buf, len, flags);
}
#endif
