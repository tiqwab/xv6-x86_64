#include "user.h"

#define SOCK_STREAM 1

#define AF_INET 2

int main(int argc, char *argv[]) {
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket: failed\n");
    return 1;
  }

  if (close(fd) < 0) {
    printf("close: failed\n");
    return 1;
  }
}
