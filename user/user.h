#ifndef XV6_x86_64_USER_H
#define XV6_x86_64_USER_H

#define NULL 0

// types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long uintptr_t;
typedef unsigned long size_t;
typedef long ssize_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef int32_t pid_t;

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
// the last entry of argv should be NULL
int exec(char *path, char **argv);
int chdir(const char *path);
int dup(int oldfd);
int getpid(void);
int open(const char *pathname, int flags);
ssize_t write(int fd, const void *buf, size_t count);
STD_WRAP(int mknod(const char *pathname, int major, int minor))
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
STD_WRAP(int mkdir(const char *pathname))
int close(int fd);

int hello(void);
STD_WRAP(int putc(char c))
int fstest(void);

// TODO: duplicated with kern/fcntl.h
#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200

// library
int printf(const char *fmt, ...);
size_t strlen(const char *s);
void *memset(void *s, int c, size_t n);

#endif /* ifndef XV6_x86_64_USER_H */
