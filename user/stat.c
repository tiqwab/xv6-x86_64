#include "user.h"

int stat(const char *path, struct stat *st) {
  int fd;
  int r;

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    return -1;
  }
  r = fstat(fd, st);
  close(fd);
  return r;
}
