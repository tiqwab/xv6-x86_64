#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip4.h"
#include "user.h"

#define BUF_LEN 1000
#define PING_DATA_SIZE (4 + 32)

static int ping_seq_num = 0;

void ping_prepare_echo(struct icmp_echo_hdr *hdr, u16_t len);
int ping_process_response(char *ipaddr, int ttl, struct icmp_echo_hdr *req_buf,
                          struct icmp_echo_hdr *res_buf, int res_len);

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in addr;
  char req_buf[BUF_LEN], res_buf[BUF_LEN];
  char *raw_ipaddr;
  struct ip_hdr *ip_hdr;
  struct icmp_echo_hdr *req_hdr, *res_hdr;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
  int n;

  if (argc != 2) {
    printf("usage: %s <ipaddr>\n", argv[0]);
    return 1;
  }

  raw_ipaddr = argv[1];

  sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sock < 0) {
    printf("socket: failed\n");
    return 1;
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(raw_ipaddr);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("connect: failed\n");
    return 1;
  }

  printf("PING %s (%s) %d bytes of data.\n", raw_ipaddr, raw_ipaddr,
         PING_DATA_SIZE);

  for (int i = 0; i < 3; i++) {
    req_hdr = (struct icmp_echo_hdr *)req_buf;
    ping_prepare_echo(req_hdr, ping_size);

    if ((n = send(sock, req_buf, ping_size, 0)) < 0) {
      printf("send: failed\n");
      return 1;
    }

    if ((n = recv(sock, res_buf, BUF_LEN, 0)) < 0) {
      printf("recv: failed\n");
      return 1;
    }

    ip_hdr = (struct ip_hdr *)res_buf;

    // skip ip header
    res_hdr = (struct icmp_echo_hdr *)(res_buf + sizeof(struct ip_hdr));
    n -= sizeof(struct ip_hdr);

    ping_process_response(raw_ipaddr, IPH_TTL(ip_hdr), req_hdr, res_hdr, n);

    sleep(1);
  }

  return 0;
}

void ping_prepare_echo(struct icmp_echo_hdr *hdr, u16_t len) {
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);
  char *p;

  memset(hdr, 0, sizeof(struct icmp_echo_hdr));

  ICMPH_TYPE_SET(hdr, ICMP_ECHO);
  ICMPH_CODE_SET(hdr, 0);
  hdr->chksum = 0;
  hdr->id = 0xAFAF; // shouldn't use the fixed value
  hdr->seqno = lwip_htons(++ping_seq_num);

  p = (char *)hdr;
  p += sizeof(struct icmp_echo_hdr);

  /* add current millis from start */
  *((unsigned int *)p) = millis_from_start();
  p += sizeof(unsigned int);

  /* fill the additional data buffer with some data */
  for (size_t i = 0; i < data_len; i++) {
    *p++ = (char)i;
  }

  hdr->chksum = inet_chksum(hdr, len);
}

int ping_process_response(char *ipaddr, int ttl, struct icmp_echo_hdr *req_hdr,
                          struct icmp_echo_hdr *res_hdr, int res_len) {
  uint16_t req_id, res_id;
  uint16_t req_seqno, res_seqno;
  char *p;
  unsigned int start_time, current_time;

  if (inet_chksum(res_hdr, res_len) == (uint16_t)-1) {
    printf("ping: checksum is illegal\n");
    return 1;
  }

  if (ICMPH_TYPE(res_hdr) != 0) {
    printf("ping: this is not echo reply: %d\n", res_hdr->type);
    return 1;
  }

  if (ICMPH_CODE(res_hdr) != 0) {
    printf("ping: code should be zero: %d\n", res_hdr->code);
    return 1;
  }

  req_id = lwip_ntohs(req_hdr->id);
  res_id = lwip_ntohs(res_hdr->id);
  if (req_hdr->id != res_hdr->id) {
    printf("ping: id is not same\n");
    return 1;
  }

  req_seqno = lwip_ntohs(req_hdr->seqno);
  res_seqno = lwip_ntohs(res_hdr->seqno);
  if (req_seqno != res_seqno) {
    printf("ping: seqno is not same\n");
    return 1;
  }

  p = (char *)res_hdr;
  p += sizeof(struct icmp_echo_hdr);
  start_time = *((unsigned int *)p);
  current_time = millis_from_start();

  printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
         res_len - sizeof(struct icmp_echo_hdr), ipaddr, res_seqno, ttl,
         current_time - start_time);
  return 0;
}
