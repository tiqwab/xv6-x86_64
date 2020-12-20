#include "socket.h"
#include "defs.h"
#include "lwip/sockets.h"

void socketclose(struct socket *sock) {
  int sockid = sock->sockid;
  kfree((char *)sock);
  lwip_close(sockid);
}
