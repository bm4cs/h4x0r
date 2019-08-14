#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>    // bzero
#include <stdarg.h>     // va_start, va_end
#include <stdlib.h>     // malloc, free
#include <stdbool.h>    // true, false
#include <signal.h>     // siginfo_t, sig_atomic_t
#include <sys/mman.h>   // mmap
#include "main.h"

#define EXPECT_GE(i, e, ...) \
    if (i < e) { __VA_ARGS__; }

#define EXPECT_SUCCESS(ret, fmt, ...) \
    EXPECT_GE(ret, 0, printf(fmt, ##__VA_ARGS__); goto End)

#define SOCKET(sockfd, domain, types, proto) \
    do { \
        sockfd = socket(domain, types, proto); \
        EXPECT_SUCCESS(sockfd, "[!] Create socket fail. %s", strerror(errno)); \
    } while(0)

#define SETSOCKOPT(ret, sockfd, level, optname, optval) \
    do { \
        int opt = optval;\
        ret = setsockopt(sockfd, level, optname, &opt, sizeof(opt)); \
        EXPECT_SUCCESS(ret, "[!] setsockopt fail. %s", strerror(errno)); \
    } while(0)

#define BIND(ret, sockfd, addr, port) \
    do { \
        struct sockaddr_in s_addr = {0}; \
        struct sockaddr sa = {0}; \
        socklen_t len = 0; \
        ret = getsockname(sockfd, &sa, &len); \
        EXPECT_SUCCESS(ret, "[!] getsockopt fail. %s", strerror(errno)); \
        s_addr.sin_family = AF_INET; \
        s_addr.sin_addr.s_addr = inet_addr(addr); \
        s_addr.sin_port = htons(port); \
        ret = bind(sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)); \
        EXPECT_SUCCESS(ret, "[!] bind fail. %s\n", strerror(errno)); \
    } while(0)

#define LISTEN(ret, sockfd, backlog) \
    do { \
        ret = listen(sockfd, backlog); \
        EXPECT_SUCCESS(ret, "[!] listen fail. %s", strerror(errno)); \
    } while(0)


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif
#define EXEC_PAGE_SIZE 4096

volatile sig_atomic_t segmentation_violation = 0;

void sigsegv_handler(int signum, siginfo_t *info, void *data)
{
        segmentation_violation = 1;
            pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    /* Bad shellcode will cause the shellcode harness in one of our child threads to segfault
       Ignore this and carry on. */
    {   struct sigaction  handler;
        sigemptyset(&handler.sa_mask);
        handler.sa_sigaction = &sigsegv_handler;
        handler.sa_flags = SA_SIGINFO;
        if (sigaction(SIGSEGV, &handler, NULL) == -1) {
            fprintf(stderr, "Cannot set SIGSEGV handler: %s.\n", strerror(errno));
            exit(1);
        }
    }

    int ret = -1, sockfd = -1, c_sockfd = -1;
    int port = 8888;
    char addr[] = "127.0.0.1";
    struct sockaddr_in c_addr = {0};
    socklen_t clen = 0;
    pthread_t t;

    puts("[+] Server is starting...");
    SOCKET(sockfd, AF_INET, SOCK_STREAM, 0);
    SETSOCKOPT(ret, sockfd, SOL_SOCKET, SO_REUSEADDR, 1);
    BIND(ret, sockfd, addr, port);
    LISTEN(ret, sockfd, 10);
    printf("[+] Server is now listening on %d\n", port);

    for(;;) {
        puts("[+] Awaiting clients...");
        c_sockfd = accept(sockfd, (struct sockaddr *)&c_addr, &clen);
        EXPECT_GE(c_sockfd, 0, continue);
        ret = pthread_create(&t, NULL, handler, (void *)&c_sockfd);
        EXPECT_GE(ret, 0, close(c_sockfd); continue);
    }
End:
    puts("[+] Server closing");
    EXPECT_GE(sockfd, 0, close(sockfd));
    ret = 0;
    return ret;
}

void *handler(void *p_sockfd)
{
    int ret = -1;
    char nop = '\x90';
    char buf[BUF_SIZE];
    int c_sockfd = *(int *)p_sockfd;
    void* shellcode = 0;

    memcpy(buf, &nop, BUF_SIZE);
    shellcode = mmap(
            NULL, 
            EXEC_PAGE_SIZE, 
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_ANONYMOUS | MAP_PRIVATE,
            0,
            0);
    printf("  [+] (Client %d) Connected\n", c_sockfd);
    //fprintfsock(c_sockfd, "You are client %d\n\n", c_sockfd);
    for (;;) {
        bzero(shellcode, EXEC_PAGE_SIZE);
        ret = recv(c_sockfd, buf, sizeof(buf) - 1, 0);
        EXPECT_GE(ret, 0, break);
        memcpy(shellcode, buf, ret);
        ((char *)shellcode)[ret] = '\xc3';
        printf("  [+] %d bytes of shellcode received. About to test.\n", ret);
        if (test(shellcode)) {
            printf("  :):):):):):):) Shellcode ran successfully!!\n");
        } else {
            printf("  :(:(:(:(:(:(:( Shellcode did not do what you expected!\n");
        }
        pthread_exit(NULL);
        /*printf("  [+] (Client %d) Sent us %d bytes\n", c_sockfd, ret);
        if( send(c_sockfd, buf, sizeof(buf) - 1, MSG_NOSIGNAL) == -1) {
            printf("  [+] (Client %d) Disconnected\n", c_sockfd);
        }*/
    }
    EXPECT_GE(c_sockfd, 0, close(c_sockfd));
    pthread_exit(NULL);
}

int (*func)();

/* Executes shellcode and then checks:
 * eax = 
 */
bool test(void* code) {
    bool flag = false;
    printf("  [+] About to test shellcode\n");
    int sum = 0;
    func = (int (*)()) code;
    asm("pushal");
    asm("mov $0x1000, %ecx");
    asm("mov $0x100, %ebx");
    sum = (*func)();
    asm("popal");
    if (sum == (0x1000 + 0x100)) {
        flag = true;
    }
    printf("  [+] Sum is: %d\n", sum);
    return flag;
}

int socket_printf(int c_sockfd, char *format, ...) {
    char msg[BUF_SIZE];
    int nbytes = 0;
    va_list argptr;

    va_start(argptr, format);
    bzero(msg, BUF_SIZE);
    vsnprintf(msg, BUF_SIZE, format, argptr);
    printf("Printing: (%s)", format);
    printf(format, argptr);
    va_end(argptr);
    msg[BUF_SIZE - 1] = 0;
    nbytes = strlen(msg);

    return send(c_sockfd, msg, nbytes, MSG_NOSIGNAL);
}

void fprintfsock(int c_sockfd, const char* format, ... )
{
    va_list a;
    va_start( a, format );
    int l = vsnprintf( 0, 0, format, a );
    char* buf = (char*) malloc( l + 1 );
    va_start( a, format );
    vsnprintf( buf, l, format, a );
    printf("Printing:");
    vprintf(format, a);
    send(c_sockfd, buf, l, 0 );
    free( buf );
}
