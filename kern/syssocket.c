#include "defs.h"
#include "file.h"
#include "lwip/sockets.h"
#include "proc.h"
#include "socket.h"

static int alloc_sockfd(int sockid) {
  struct file *f = 0;
  struct socket *sock = 0;
  int fd = 0;

  if ((f = filealloc()) == 0) {
    goto bad;
  }

  if ((fd = fdalloc(f)) < 0) {
    goto bad;
  }

  if ((sock = (struct socket *)kalloc()) == 0) {
    goto bad;
  }
  sock->sockid = sockid;

  f->type = FD_SOCKET;
  f->off = 0;
  f->readable = 1;
  f->writable = 1;
  f->sock = sock;

  return fd;

bad:
  if (sock) {
    kfree((char *)sock);
  }
  if (fd) {
    myproc()->ofile[fd] = 0;
  }
  if (f) {
    fileclose(f);
  }
  return -1;
}

int sys_socket(void) {
  int domain, type, protocol;
  int sockid, sockfd;

  if (argint(0, &domain) < 0 || argint(1, &type) < 0 ||
      argint(2, &protocol) < 0) {
    return -1;
  }

  if ((sockid = lwip_socket(domain, type, protocol)) < 0) {
    return -1;
  }
  if ((sockfd = alloc_sockfd(sockid)) < 0) {
    lwip_close(sockid);
    return -1;
  }
  return sockfd;
}