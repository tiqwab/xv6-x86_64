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
int exec(char *path, char **argv);
int getpid(void);

int hello(void);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
int putc(char c);
#pragma GCC diagnostic pop

// library
int printf(const char *fmt, ...);

#endif /* ifndef XV6_x86_64_USER_H */
