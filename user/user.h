#ifndef XV6_x86_64_USER_H
#define XV6_x86_64_USER_H

#define NULL 0

#include "inc/dir.h"
#include "inc/fcntl.h"
#include "inc/stat.h"
#include "inc/stdarg.h"
#include "inc/string.h"
#include "inc/types.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"

// wrap functions which exist in stds, but have different signatures.
#define STD_WRAP(f)                                                            \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wbuiltin-declaration-mismatch\"") f;  \
  _Pragma("GCC diagnostic pop")

// system calls
int fork(void);
STD_WRAP(void exit(void))
int wait(void);
int pipe(int pipefd[2]);
ssize_t read(int fd, void *buf, size_t count);
STD_WRAP(int kill(pid_t pid))
// the last entry of argv should be NULL
int exec(char *path, char **argv);
int fstat(int fd, struct stat *statbuf);
int chdir(const char *path);
int dup(int oldfd);
int getpid(void);
void *sbrk(intptr_t increment);
int sleep(int n);
int open(const char *pathname, int flags);
ssize_t write(int fd, const void *buf, size_t count);
STD_WRAP(int mknod(const char *pathname, int major, int minor))
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
STD_WRAP(int mkdir(const char *pathname))
int close(int fd);
int socket(int domain, int type, int protocol);
int bind(int s, const struct sockaddr *name, socklen_t namelen);
int listen(int s, int backlog);
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int connect(int s, struct sockaddr *addr, socklen_t addrlen);
int tcpip_worker(void);

// library
int printf(const char *fmt, ...);
int dprintf(int fd, const char *fmt, ...);
int vdprintf(int fd, const char *fmt, va_list va);
STD_WRAP(char *gets(char *buf, int max))
void *malloc(size_t nbytes);
void free(void *ap);
int stat(const char *path, struct stat *buf);

#endif /* ifndef XV6_x86_64_USER_H */
