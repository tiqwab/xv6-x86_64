#include "lwip/etharp.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"

#include "string.h"

#define IP "10.0.2.15"
#define MASK "255.255.255.0"
#define GATEWAY "10.0.2.2"

static struct netif nif;

static void low_level_init(struct netif *netif) {
  int r;

  netif->hwaddr_len = 6;
  netif->mtu = 1500;
  netif->flags = NETIF_FLAG_BROADCAST;

  // MAC address is hardcoded to eliminate a system call
  netif->hwaddr[0] = 0x52;
  netif->hwaddr[1] = 0x54;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0x12;
  netif->hwaddr[4] = 0x34;
  netif->hwaddr[5] = 0x56;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p) {
  // int r = sys_page_alloc(0, (void *)PKTMAP, PTE_U|PTE_W|PTE_P);
  // if (r < 0)
  // panic("jif: could not allocate page of memory");
  // struct jif_pkt *pkt = (struct jif_pkt *)PKTMAP;

  // struct jif *jif;
  // jif = netif->state;

  // char *txbuf = pkt->jp_data;
  // int txsize = 0;
  // struct pbuf *q;
  // for (q = p; q != NULL; q = q->next) {
  // /* Send the data from the pbuf to the interface, one pbuf at a
  //    time. The size of the data in each pbuf is kept in the ->len
  //    variable. */

  // if (txsize + q->len > 2000)
  //     panic("oversized packet, fragment %d txsize %d\n", q->len, txsize);
  // memcpy(&txbuf[txsize], q->payload, q->len);
  // txsize += q->len;
  // }

  // pkt->jp_len = txsize;

  // // char b[300];
  // // memcpy(b, pkt->jp_data, pkt->jp_len);
  // // b[pkt->jp_len] = '\0';
  // // cprintf("[low_level_input] data: %s, len: %d\n", &b[60], pkt->jp_len);

  // ipc_send(jif->envid, NSREQ_OUTPUT, (void *)pkt, PTE_P|PTE_W|PTE_U);
  // sys_page_unmap(0, (void *)pkt);

  cprintf("low_level_output\n");

  return ERR_OK;
}

static err_t xif_output(struct netif *netif, struct pbuf *p,
                        const struct ip4_addr *ipaddr) {
  /* resolve hardware address, then send (or queue) packet */
  return etharp_output(netif, p, ipaddr);
}

static err_t xif_init(struct netif *netif) {
  // netif->state = jif;
  netif->output = xif_output;
  netif->linkoutput = low_level_output;
  memcpy(&netif->name[0], "en", 2);

  low_level_init(netif);
  // etharp_init();

  // qemu user-net is dumb; if the host OS does not send and ARP request
  // first, the qemu will send packets destined for the host using the mac
  // addr 00:00:00:00:00; do a arp request for the user-net NAT at 10.0.2.2
  ip4_addr_t ipaddr;
  ipaddr.addr = inet_addr(GATEWAY);
  etharp_query(netif, &ipaddr, 0);

  return ERR_OK;
}

void net_init() {
  ip4_addr_t ipaddr, netmask, gateway;

  tcpip_init(NULL, NULL); // initialize lwip

  ipaddr.addr = inet_addr(IP);
  netmask.addr = inet_addr(MASK);
  gateway.addr = inet_addr(GATEWAY);

  netif_add(&nif, &ipaddr, &netmask, &gateway, NULL, xif_init, tcpip_input);

  netif_set_default(&nif);
  netif_set_up(&nif);
  netif_set_link_up(&nif);
}
