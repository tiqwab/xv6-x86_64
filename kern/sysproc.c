#include "defs.h"
#include "proc.h"
#include "types.h"

int64_t sys_getpid(void) { return (int64_t)myproc()->pid; }
