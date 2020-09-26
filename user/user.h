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

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef int32_t pid_t;

// system calls
int fork(void);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
void exit(void);
#pragma GCC diagnostic pop
int wait(void);
// the last entry of argv should be NULL
int exec(char *path, char **argv);
int getpid(void);
int open(const char *pathname, int flags);
int close(int fd);

int hello(void);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
int putc(char c);
#pragma GCC diagnostic pop
int fstest(void);

// TODO: duplicated with kern/fcntl.h
#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200

// library
int printf(const char *fmt, ...);

#endif /* ifndef XV6_x86_64_USER_H */
