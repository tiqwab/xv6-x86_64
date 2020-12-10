#ifndef XV6_X86_64_ARCH_ASSERT_H
#define XV6_X86_64_ARCH_ASSERT_H

#define assert(x)                                                              \
  do {                                                                         \
    if (!(x))                                                                  \
      panic("assertion failed: %s\n", #x);                                     \
  } while (0)

#endif /* XV6_X86_64_ARCH_ASSERT_H */
