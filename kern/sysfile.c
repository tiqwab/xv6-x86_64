#include "defs.h"
#include "param.h"
#include "types.h"

int64_t sys_exec(void) {
  char *path, *argv[MAXARG];
  int i;
  uint64_t uargv, uarg;

  if (argstr(0, &path) < 0 || arg(1, &uargv) < 0) {
    return -1;
  }

  memset(argv, 0, sizeof(argv));

  for (i = 0;; i++) {
    if (i >= MAXARG) {
      return -1;
    }
    if (fetchint(uargv + sizeof(uintptr_t) * i, (uint64_t *)&uarg) < 0) {
      return -1;
    }
    if (uarg == 0) {
      argv[i] = 0;
      break;
    }
    if (fetchstr(uarg, &argv[i]) < 0) {
      return -1;
    }
  }

  return exec(path, argv);
}