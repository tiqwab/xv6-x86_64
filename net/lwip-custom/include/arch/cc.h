#ifndef XV6_X86_64_ARCH_CC_H
#define XV6_X86_64_ARCH_CC_H

#include "arch/kern.h"
#include "inc/types.h"

typedef uint32_t u32_t;
typedef int32_t s32_t;

typedef uint64_t u64_t;
typedef int64_t s64_t;

typedef uint16_t u16_t;
typedef int16_t s16_t;

typedef uint8_t u8_t;
typedef int8_t s8_t;

typedef uintptr_t mem_ptr_t;

typedef long ptrdiff_t;

#define INT_MIN (-INT_MAX - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U

#define LONG_MIN (-LONG_MAX - 1L)
#define LONG_MAX 9223372036854775807L
#define ULONG_MAX 18446744073709551615UL

#define SSIZE_MAX LONG_MAX

// We don't provide stddef.h, so define size_t by ourselves (defined in
// inc/types.h). See net/lwip/include/lwip/arch.h L110.
#define LWIP_NO_STDDEF_H 1

// We don't provide stdint.h, so define types such as u8_t by ourselves (
// defined in inc/types.h). See net/lwip/include/lwip/arch.h L118.
#define LWIP_NO_STDINT_H 1

// We don't provide inttypes.h, so define macros such as X16_F by ourselves (
// defined below). See net/lwip/include/lwip/arch.h L147.
#define LWIP_NO_INTTYPES_H 1

// We don't provide limits.h
// See net/lwip/include/lwip/arch.h L175.
#define LWIP_NO_LIMITS_H 1

// We don't provide unistd.h, so define macros such as SSIZE_MAX by ourselves (
// defined above). See net/lwip/include/lwip/arch.h L188.
#define LWIP_NO_UNISTD_H 1

// We don't provide ctype.h
// See net/lwip/include/lwip/arch.h L175.
#define LWIP_NO_CTYPE_H 1

// We define socket library functions.
// See net/lwip/include/lwip/opt.h L1982
#define LWIP_COMPAT_SOCKETS 0

#define S8_F "d"
#define U8_F "u"
#define X8_F "x"

#define S16_F "d"
#define U16_F "u"
#define X16_F "x"

#define S32_F "d"
#define U32_F "u"
#define X32_F "x"

#define SZT_F "u"

#define LWIP_PLATFORM_DIAG(x) cprintf x
#define LWIP_PLATFORM_ASSERT(x) panic(x)

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif /* ifndef XV6_X86_64_ARCH_CC_H */
