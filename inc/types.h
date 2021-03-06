#ifndef XV6_X86_64_TYPES_H
#define XV6_X86_64_TYPES_H

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#ifndef USE_STD

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long uintptr_t;
typedef unsigned long size_t;
typedef long ssize_t;
typedef long long intptr_t;

#define NULL 0

#endif

typedef uint64_t pte_t;
typedef int32_t pid_t;

#endif /* XV6_X86_64_TYPES_H */
