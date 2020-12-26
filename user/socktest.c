#include "user.h"

#define SOCK_STREAM 1

#define AF_INET 2

#define PORT 12345

int do_client() {
  int fd;
  struct sockaddr_in server;

  // wait for server set up
  sleep(1);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("[client] socket: failed\n");
    return 1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = lwip_htons(PORT);

  if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("[client] connect: failed\n");
    return 1;
  }

  return 0;
}

int do_server() {
  int server_fd;
  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t client_len;
  int client_fd;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("[server] socket: failed\n");
    return 1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = lwip_htonl(INADDR_ANY);
  server.sin_port = lwip_htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("[server] bind: failed\n");
    return 1;
  }

  if (listen(server_fd, 5) < 0) {
    printf("[server] listen: failed\n");
    return 1;
  }

  client_len = sizeof(client);
  if ((client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len)) <
      0) {
    printf("[server] accept: failed\n");
    return 1;
  }
  printf("[server] client connected from %s:%d\n", inet_ntoa(client.sin_addr),
         client.sin_port);

  if (close(client_fd) < 0) {
    printf("[server] close: failed for client\n");
  }

  if (close(server_fd) < 0) {
    printf("[server] close: failed for server\n");
    return 1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int pid;

  if ((pid = fork()) < 0) {
    printf("fork: failed\n");
    return 1;
  } else if (pid == 0) {
    // child for client
    do_client();
  } else {
    // parent for server
    do_server();
    wait();
  }

  return 0;
}
