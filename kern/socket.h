#ifndef XV6_X86_64_SOCKET_H
#define XV6_X86_64_SOCKET_H

struct socket {
  int sockid;
};

void socketclose(struct socket *sock);

#endif /* XV6_X86_64_SOCKET_H */
