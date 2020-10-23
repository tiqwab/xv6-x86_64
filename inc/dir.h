#ifndef XV6_X86_64_DIR_H
#define XV6_X86_64_DIR_H

#include "inc/types.h"

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

#endif /* ifndef XV6_X86_64_DIR_H */
