#include "user.h"

#define SOCK_STREAM 1

#define AF_INET 2

int main(int argc, char *argv[]) {
  int fd;
  struct sockaddr_in server;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket: failed\n");
    return 1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = lwip_htonl(INADDR_ANY);
  server.sin_port = lwip_htons(12345);

  if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("bind: failed\n");
    return 1;
  }

  if (listen(fd, 5) < 0) {
    printf("listen: failed\n");
    return 1;
  }

  if (close(fd) < 0) {
    printf("close: failed\n");
    return 1;
  }
}
