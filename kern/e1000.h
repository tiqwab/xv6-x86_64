#ifndef XV6_X86_64_E1000_H
#define XV6_X86_64_E1000_H

#include "inc/types.h"
#include "pci.h"

#define PCI_E1000_CLASS 0x8086
#define PCI_E1000_SUBCLASS 0x100e

#define PCI_E1000_REG_OFFSET_TDBAL 0x03800
#define PCI_E1000_REG_OFFSET_TDBAH 0x03804
#define PCI_E1000_REG_OFFSET_TDLEN 0x03808
#define PCI_E1000_REG_OFFSET_TDH 0x03810
#define PCI_E1000_REG_OFFSET_TDT 0x03818
#define PCI_E1000_REG_OFFSET_TCTL 0x00400
#define PCI_E1000_REG_OFFSET_TIPG 0x00410

#define PCI_E1000_NUM_TX_DESC 64
#define MAX_ETHERNET_PACKET_LEN 2048

#define TX_DESC_CMD_EOP (1 << 0)
#define TX_DESC_CMD_RS (1 << 3)
#define TX_DESC_STATUS_DD (1 << 0)

struct tx_desc {
  uint64_t addr;
  uint16_t length;
  uint8_t cso;
  uint8_t cmd;
  uint8_t status;
  uint8_t css;
  uint16_t special;
};

#define PCI_E1000_REG_OFFSET_RAL0 0x05400
#define PCI_E1000_REG_OFFSET_RAH0 0x05404
#define PCI_E1000_REG_OFFSET_MTA 0x05200
#define PCI_E1000_REG_OFFSET_IMS 0x000D0
#define PCI_E1000_REG_OFFSET_RDBAL 0x02800
#define PCI_E1000_REG_OFFSET_RDBAH 0x02804
#define PCI_E1000_REG_OFFSET_RDLEN 0x02808
#define PCI_E1000_REG_OFFSET_RDH 0x02810
#define PCI_E1000_REG_OFFSET_RDT 0x02818
#define PCI_E1000_REG_OFFSET_RCTL 0x00100

#define PCI_E1000_RAH_AV (1 << 31)

#define RX_DESC_STATUS_DD (1 << 0)

#define PCI_E1000_NUM_RX_DESC 128

struct rx_desc {
  uint64_t addr;
  uint16_t length;
  uint16_t checksum;
  uint8_t status;
  uint8_t errors;
  uint16_t special;
};

int attachfn_e1000(struct pci_func *pcif);

/*
 * Return 0 if successful, otherwise < 0.
 * This might drop packets if the buffer is full.
 */
int transmit_packet(char *data, uint16_t len);

/*
 * Return 0 if successful, otherwise < 0.
 * Return -E_NO_PACKET if there is no packet to receive.
 */
int receive_packet(char *buf, uint16_t *len);

#endif /* XV6_X86_64_E1000_H */
