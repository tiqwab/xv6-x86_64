#include "user.h"

int main(int argc, char *argv[]) {
  fstest();

  int fd;
  ssize_t n;
  char *file_name = "/foo";
  char *message = "hello from write";
  char buf[128];

  if ((fd = open("/foo", O_CREATE | O_RDWR)) < 0) {
    printf("failed to open %s\n", file_name);
    return 1;
  }

  if ((n = write(fd, message, strlen(message))) < 0 ||
      (size_t)n != strlen(message)) {
    printf("failed to write to %s\n", file_name);
    return 1;
  }

  if (read(fd, buf, 128) < 0) {
    printf("failed to read from %s\n", file_name);
    return 1;
  }
  printf("%s\n", message);

  if (close(fd) < 0) {
    printf("failed to close %s\n", file_name);
    return 1;
  }

  return 0;
}
