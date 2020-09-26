#include "user.h"

int main(int argc, char *argv[]) {
  fstest();

  int fd;
  if ((fd = open("/foo", O_CREATE)) < 0) {
    printf("failed to open %s\n", "/foo");
  }
  close(fd);

  return 0;
}
