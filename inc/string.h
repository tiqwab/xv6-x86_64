#ifndef XV6_X86_64_STRING_H
#define XV6_X86_64_STRING_H

#include "inc/types.h"

int memcmp(const void *v1, const void *v2, size_t n);
void *memset(void *, int, size_t);
void *memmove(void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
char *strncpy(char *s, const char *t, size_t n);
char *safestrcpy(char *s, const char *t, int n);
size_t strlen(const char *s);
int strncmp(const char *p, const char *q, size_t n);
char *strchr(const char *s, int c);

#endif /* ifndef XV6_X86_64_STRING_H */
