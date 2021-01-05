#ifndef XV6_X86_64_LWIPOPTS_H
#define XV6_X86_64_LWIPOPTS_H

#include "lwip/debug.h"

// ref. https://lwip.fandom.com/wiki/Porting_for_an_OS#cc.h
// #define ETH_PAD_SIZE 2
#define MEM_ALIGNMENT 4

// We don't provide errno.h, so use it defined in lwip.
// See net/lwip/include/lwip/errno.h
#define LWIP_PROVIDE_ERRNO 1

// Enable debug if LWIP_DEBUG is defined
// We have to enable debug for each functions?
#ifdef LWIP_DEBUG
#define ETHARP_DEBUG LWIP_DBG_ON
#define NETIF_DEBUG LWIP_DBG_ON
#define PBUF_DEBUG LWIP_DBG_ON
#define API_LIB_DEBUG LWIP_DBG_ON
#define API_MSG_DEBUG LWIP_DBG_ON
#define SOCKETS_DEBUG LWIP_DBG_ON
#define ICMP_DEBUG LWIP_DBG_ON
#define IGMP_DEBUG LWIP_DBG_ON
#define INET_DEBUG LWIP_DBG_ON
#define IP_DEBUG LWIP_DBG_ON
#define IP_REASS_DEBUG LWIP_DBG_ON
#define RAW_DEBUG LWIP_DBG_ON
#define MEM_DEBUG LWIP_DBG_ON
#define MEMP_DEBUG LWIP_DBG_ON
#define SYS_DEBUG LWIP_DBG_ON
#define TIMERS_DEBUG LWIP_DBG_ON
#define TCP_DEBUG LWIP_DBG_ON
#define TCP_INPUT_DEBUG LWIP_DBG_ON
#define TCP_FR_DEBUG LWIP_DBG_ON
#define TCP_RTO_DEBUG LWIP_DBG_ON
#define TCP_CWND_DEBUG LWIP_DBG_ON
#define TCP_WND_DEBUG LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_RST_DEBUG LWIP_DBG_ON
#define TCP_QLEN_DEBUG LWIP_DBG_ON
#define UDP_DEBUG LWIP_DBG_ON
#define TCPIP_DEBUG LWIP_DBG_ON
#define SLIP_DEBUG LWIP_DBG_ON
#define DHCP_DEBUG LWIP_DBG_ON
#define AUTOIP_DEBUG LWIP_DBG_ON
#define ACD_DEBUG LWIP_DBG_ON
#define DNS_DEBUG LWIP_DBG_ON
#define IP6_DEBUG LWIP_DBG_ON
#define DHCP6_DEBUG LWIP_DBG_ON
#endif

/**
 * Define loopback interface
 * See net/lwip/include/lwip/opt.h L.1727
 */
// #define LWIP_HAVE_LOOPIF 1

/**
 * LWIP_NETIF_LOOPBACK_MULTITHREADING: Indicates whether threading is enabled in
 * the system, as netifs must change how they behave depending on this setting
 * for the LWIP_NETIF_LOOPBACK option to work.
 * Setting this is needed to avoid reentering non-reentrant functions like
 * tcp_input().
 *    LWIP_NETIF_LOOPBACK_MULTITHREADING==1: Indicates that the user is using a
 *       multithreaded environment like tcpip.c. In this case, netif->input()
 *       is called directly.
 *    LWIP_NETIF_LOOPBACK_MULTITHREADING==0: Indicates a polling (or NO_SYS)
 * setup. The packets are put on a list and netif_poll() must be called in the
 * main application loop.
 *
 * See net/lwip/include/lwip/opt.h L.1767
 */
// #define LWIP_NETIF_LOOPBACK_MULTITHREADING 0

/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 *
 * See net/lwip/include/lwip/opt.h L.1742
 */
#define LWIP_NETIF_LOOPBACK 1

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 *
 * See net/lwip/include/lwip/opt.h L.895
 */
#define LWIP_RAW 1

/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP *tries* to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 * Beware that this might involve CPU-memcpy before transmitting that would not
 * be needed without this flag! Use this only if you need to!
 *
 * ATTENTION: a driver should *NOT* rely on getting single pbufs but check TX
 * pbufs for being in one piece. If not, @ref pbuf_clone can be used to get
 * a single pbuf:
 *   if (p->next != NULL) {
 *     struct pbuf *q = pbuf_clone(PBUF_RAW, PBUF_RAM, p);
 *     if (q == NULL) {
 *       return ERR_MEM;
 *     }
 *     p = q; ATTENTION: do NOT free the old 'p' as the ref belongs to the
 * caller!
 *   }
 *
 * See net/lwip/include/lwip/opt.h L.1697
 */
#define LWIP_NETIF_TX_SINGLE_PBUF 1

#endif /* ifndef XV6_X86_64_LWIPOPTS_H */
