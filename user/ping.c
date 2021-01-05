#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "user.h"

#define BUF_LEN 2000
#define PING_DATA_SIZE 32

static int ping_seq_num = 0;

static void ping_prepare_echo(struct icmp_echo_hdr *hdr, u16_t len) {
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  memset(hdr, 0, sizeof(struct icmp_echo_hdr));

  ICMPH_TYPE_SET(hdr, ICMP_ECHO);
  ICMPH_CODE_SET(hdr, 0);
  hdr->chksum = 0;
  hdr->id = 0xAFAF; // shouldn't use the fixed value
  hdr->seqno = lwip_htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for (size_t i = 0; i < data_len; i++) {
    ((char *)hdr)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  hdr->chksum = inet_chksum(hdr, len);
}

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in addr;
  char buf[BUF_LEN];
  struct icmp_echo_hdr *hdr;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
  int n;

  if (argc != 2) {
    printf("usage: %s <ipaddr>\n", argv[0]);
    return 1;
  }

  sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sock < 0) {
    printf("socket: failed\n");
    return 1;
  }

  hdr = (struct icmp_echo_hdr *)buf;
  ping_prepare_echo(hdr, ping_size);

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("connect: failed\n");
    return 1;
  }

  if ((n = send(sock, buf, ping_size, 0)) < 0) {
    printf("send: failed\n");
    return 1;
  }

  return 0;
}

// #include <stdio.h>
//
// #include <sys/socket.h>
// #include <netinet/in.h>
//
// #include <netinet/ip.h>
// #include <netinet/ip_icmp.h>
//
// /*
//  * チェックサムを計算する関数です。
//  * ICMPヘッダのチェックサムフィールドを埋めるために利用します。
//  * IPヘッダなどでも全く同じ計算を利用するので、
//  * IPヘッダのチェックサム計算用としても利用できます。
//  */
// unsigned short
// checksum(unsigned short *buf, int bufsz)
// {
//   unsigned long sum = 0;
//
//   while (bufsz > 1) {
//     sum += *buf;
//     buf++;
//     bufsz -= 2;
//   }
//
//   if (bufsz == 1) {
//     sum += *(unsigned char *)buf;
//   }
//
//   sum = (sum & 0xffff) + (sum >> 16);
//   sum = (sum & 0xffff) + (sum >> 16);
//
//   return ~sum;
// }
//
// /* main 文はここからです。*/
// int
// main(int argc, char *argv[])
// {
//   int sock;
//   struct icmphdr hdr;
//   struct sockaddr_in addr;
//   int n;
//
//   char buf[2000];
//   struct icmphdr *icmphdrptr;
//   struct iphdr *iphdrptr;
//
//   if (argc != 2) {
//     printf("usage : %s IPADDR\n", argv[0]);
//     return 1;
//   }
//
//   addr.sin_family = AF_INET;
//   addr.sin_addr.s_addr = inet_addr(argv[1]);
//
//   /* RAWソケットを作成します */
//   sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
//   if (sock < 0) {
//     perror("socket");
//     return 1;
//   }
//
//   memset(&hdr, 0, sizeof(hdr));
//
//   /* ICMPヘッダを用意します */
//   hdr.type = ICMP_ECHO;
//   hdr.code = 0;
//   hdr.checksum = 0;
//   hdr.un.echo.id = 0;
//   hdr.un.echo.sequence = 0;
//
//   /* ICMPヘッダのチェックサムを計算します */
//   hdr.checksum = checksum((unsigned short *)&hdr, sizeof(hdr));
//
//   /* ICMPヘッダだけのICMPパケットを送信します */
//   /* ICMPデータ部分はプログラムを簡潔にするために省いています */
//   n = sendto(sock,
//              (char *)&hdr, sizeof(hdr),
//              0, (struct sockaddr *)&addr, sizeof(addr));
//   if (n < 1) {
//     perror("sendto");
//   }
//
//   /* ICMP送信部分はここまでです*/
//   /* ここから下はICMP ECHO REPLY受信部分になります */
//
//   memset(buf, 0, sizeof(buf));
//
//   /* 相手ホストからのICMP ECHO REPLYを待ちます */
//   n = recv(sock, buf, sizeof(buf), 0);
//   if (n < 1) {
//     perror("recv");
//   }
//
//   /* 受信データからIPヘッダ部分へのポインタを取得します */
//   iphdrptr = (struct iphdr *)buf;
//
//   /*
//    * 本当はIPヘッダを調べて
//    * パケットがICMPパケットかどうか調べるべきです
//    */
//
//   /* 受信データからICMPヘッダ部分へのポインタを取得します */
//   icmphdrptr = (struct icmphdr *)(buf + (iphdrptr->ihl * 4));
//
//   /* ICMPヘッダからICMPの種類を特定します */
//   if (icmphdrptr->type == ICMP_ECHOREPLY) {
//     printf("received ICMP ECHO REPLY\n");
//   } else {
//     printf("received ICMP %d\n", icmphdrptr->type);
//   }
//
//   /* 終了 */
//   close(sock);
//
//   return 0;
// }
