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

static int fd_to_sock(int fd) {
  struct proc *curproc = myproc();

  struct file *f = curproc->ofile[fd];
  if (f->type != FD_SOCKET) {
    return -1;
  }

  return f->sock->sockid;
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

int sys_bind(void) {
  int s, fd;
  struct sockaddr *name;
  socklen_t namelen;

  if (argint(0, &fd) < 0) {
    return -1;
  }
  if (argint(2, (int *)&namelen) < 0) {
    return -1;
  }
  if (argptr(1, (char **)&name, namelen) < 0) {
    return -1;
  }

  s = fd_to_sock(fd);
  if (s < 0) {
    return s;
  }

  return lwip_bind(s, name, namelen);
}

int sys_listen(void) {
  int s, fd, backlog;

  if (argint(0, &fd) < 0 || argint(1, &backlog) < 0) {
    return -1;
  }

  s = fd_to_sock(fd);
  if (s < 0) {
    return s;
  }

  return lwip_listen(s, backlog);
}

int sys_accept(void) {
  int s, fd;
  struct sockaddr *addr;
  socklen_t *addrlen;

  if (argint(0, &fd) < 0) {
    return -1;
  }
  if (argptr(2, (char **)&addrlen, sizeof(int)) < 0) {
    return -1;
  }
  if (argptr(1, (char **)&addr, (int)*addrlen) < 0) {
    return -1;
  }

  s = fd_to_sock(fd);
  if (s < 0) {
    return s;
  }

  return lwip_accept(s, addr, addrlen);
}
