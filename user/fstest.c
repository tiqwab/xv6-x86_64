#include "user.h"

int main(int argc, char *argv[]) {
  fstest();

  int fd1, fd2;
  ssize_t n;
  char *dir_name = "foo";
  char *file_name = "bar.txt";
  char *message = "hello from write";
  char buf[128];

  if (mkdir(dir_name) < 0) {
    printf("failed to mkdir %s\n", dir_name);
    return 1;
  }

  if ((fd1 = open("/foo/bar.txt", O_CREATE | O_RDWR)) < 0) {
    printf("failed to open %s\n", "/foo/bar.txt");
    return 1;
  }

  if ((n = write(fd1, message, strlen(message))) < 0 ||
      (size_t)n != strlen(message)) {
    printf("failed to write to %s\n", file_name);
    return 1;
  }

  if (link("/foo/bar.txt", "/foo/baz.txt") < 0) {
    printf("failed to link. old: %s, new: %s\n", "/foo/bar.txt",
           "/foo/baz.txt");
    return 1;
  }

  if ((fd2 = open("/foo/baz.txt", O_RDONLY)) < 0) {
    printf("failed to open %s\n", "/foo/baz.txt");
    return 1;
  }

  if (read(fd2, buf, 128) < 0) {
    printf("failed to read from %s\n", file_name);
    return 1;
  }
  printf("%s\n", message);

  close(fd2);
  close(fd1);

  return 0;
}
