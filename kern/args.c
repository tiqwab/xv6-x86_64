#include "defs.h"

// The size of args must be equal to or larger than 5.
__inline__ void prepare_args(void *args[]) {
  __asm__ volatile("mov %%rsi,%0" : "=a"(args[0]) : :);
  __asm__ volatile("mov %%rdx,%0" : "=a"(args[1]) : :);
  __asm__ volatile("mov %%rcx,%0" : "=a"(args[2]) : :);
  __asm__ volatile("mov %%r8,%0" : "=a"(args[3]) : :);
  __asm__ volatile("mov %%r9,%0" : "=a"(args[4]) : :);
}
