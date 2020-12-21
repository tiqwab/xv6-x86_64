#include "user.h"

#define SOCK_STREAM 1

#define AF_INET 2

int main(int argc, char *argv[]) {
  int server_fd;
  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t client_len;
  int client_fd;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket: failed\n");
    return 1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = lwip_htonl(INADDR_ANY);
  server.sin_port = lwip_htons(12345);

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("bind: failed\n");
    return 1;
  }

  if (listen(server_fd, 5) < 0) {
    printf("listen: failed\n");
    return 1;
  }

  client_len = sizeof(client);
  if ((client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len)) <
      0) {
    printf("accept: failed\n");
    return 1;
  }

  if (close(client_fd) < 0) {
    printf("close: failed for client\n");
  }

  if (close(server_fd) < 0) {
    printf("close: failed for server\n");
    return 1;
  }
}
