#include "lwip/etharp.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"

#include "e1000.h"
#include "string.h"

#define IP "10.0.2.15"
#define MASK "255.255.255.0"
#define GATEWAY "10.0.2.2"

static struct netif nif;
static int xif_initialized = 0;

static void low_level_init(struct netif *netif) {
  int r;

  netif->hwaddr_len = 6;
  netif->mtu = 1500;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

  // MAC address is hardcoded to eliminate a system call
  netif->hwaddr[0] = 0x52;
  netif->hwaddr[1] = 0x54;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0x12;
  netif->hwaddr[4] = 0x34;
  netif->hwaddr[5] = 0x56;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p) {
  cprintf("low_level_output: len: %d\n", p->len);
  transmit_packet(p->payload, p->len);

  return ERR_OK;
}

static struct pbuf *low_level_input() {
  char buf[1518]; /* max packet size including VLAN excluding CRC */
  uint16_t len;
  struct pbuf *p;

  if (receive_packet(buf, &len) < 0) {
    // cprintf("low_level_input: failed to receive packet\n");
    return NULL;
  }

  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  if (p == NULL) {
    cprintf("low_level_input: failed to allocate pbuf\n");
    return NULL;
  }

  pbuf_take(p, buf, len);
  return p;

  // struct jif_pkt *pkt = (struct jif_pkt *)va;
  // s16_t len = pkt->jp_len;

  // struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  // if (p == 0) {
  //   return 0;
  // }

  // /* We iterate over the pbuf chain until we have read the entire
  //  * packet into the pbuf. */
  // void *rxbuf = (void *) pkt->jp_data;
  // int copied = 0;
  // struct pbuf *q;
  // for (q = p; q != NULL; q = q->next) {
  // /* Read enough bytes to fill this pbuf in the chain. The
  //  * available data in the pbuf is given by the q->len
  //  * variable. */
  // int bytes = q->len;
  // if (bytes > (len - copied)) {
  //     bytes = len - copied;
  // }
  // memcpy(q->payload, rxbuf + copied, bytes);
  // copied += bytes;
  // }

  // return p;
}

static err_t xif_output(struct netif *netif, struct pbuf *p,
                        const struct ip4_addr *ipaddr) {
  /* resolve hardware address, then send (or queue) packet */
  return etharp_output(netif, p, ipaddr);
}

void xif_input() {
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  if (!xif_initialized) {
    return;
  }

  /* move received packet into a new pbuf */
  p = low_level_input();

  /* no packet could be read, silently ignore this */
  if (p == NULL) {
    return;
  }

  if (nif.input(p, &nif) != ERR_OK) {
    cprintf("xif_input: netif input error\n");
  }

  // pbuf is freed in lwip?
  // end:
  //   pbuf_free(p);

  /* points to packet payload, which starts with an Ethernet header */
  // ethhdr = p->payload;

  // switch (htons(ethhdr->type)) {
  // case ETHTYPE_IP:
  //   /* update ARP table */
  //   etharp_ip_input(netif, p);
  //   /* skip Ethernet header */
  //   pbuf_header(p, -(int)sizeof(struct eth_hdr));
  //   /* pass to network layer */
  //   netif->input(p, netif);
  //   break;

  // case ETHTYPE_ARP:
  //   /* pass p to ARP module  */
  //   etharp_arp_input(netif, jif->ethaddr, p);
  //   break;

  // default:
  //   pbuf_free(p);
  // }
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

void tcpip_thread(void *arg);

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

  xif_initialized = 1;

  tcpip_thread(NULL);
}
