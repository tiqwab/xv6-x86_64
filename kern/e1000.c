#include "e1000.h"
#include "defs.h"
#include "inc/types.h"
#include "lwip/err.h"
#include "memlayout.h"

uintptr_t physaddr_e1000;
uint32_t size_e1000;
volatile uint32_t *vaddr_e1000;

#define PCI_E1000_REG_VALUE(offset)                                            \
  (*((uint32_t *)(((char *)vaddr_e1000) + (offset))))
#define PCI_E1000_REG_ADDR(offset)                                             \
  ((uint32_t *)(((char *)vaddr_e1000) + (offset)))

static struct tx_desc tx_descs[PCI_E1000_NUM_TX_DESC];
static char tx_buf[PCI_E1000_NUM_TX_DESC][MAX_ETHERNET_PACKET_LEN];

static struct rx_desc rx_descs[PCI_E1000_NUM_RX_DESC];
static char rx_buf[PCI_E1000_NUM_RX_DESC][MAX_ETHERNET_PACKET_LEN];

int attachfn_e1000(struct pci_func *pcif) {
  pci_func_enable(pcif);
  physaddr_e1000 = (uintptr_t)pcif->reg_base[0];
  size_e1000 = pcif->reg_size[0];
  vaddr_e1000 = mmio_map_region(physaddr_e1000, size_e1000);

  // debug
  {
    volatile uint32_t *status = PCI_E1000_REG_ADDR(8);
    cprintf("[attachfn_e1000] phyaddr_e1000: 0x%x, vaddr_e1000: 0x%x, "
            "size_e1000: 0x%x\n",
            physaddr_e1000, vaddr_e1000, size_e1000);
    cprintf("device status (ref. 13.4.2): 0x%x\n", *status);
  }

  uint32_t *p;

  // receive

  for (int i = 0; i < PCI_E1000_NUM_RX_DESC; i++) {
    rx_descs[i].addr = (uint64_t)V2P(&rx_buf[i]);
  }

  // RAL
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RAL0);
  *p = 0x12005452;

  // RAH
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RAH0);
  *p = (0x00005634) | PCI_E1000_RAH_AV;

  // MTA
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_MTA);
  for (int i = 0; i < 128; i++) {
    *(p + i) = 0;
  }

  // IMS
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_MTA);
  *p = 0;

  // RDBAL
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RDBAL);
  *p = (uint32_t)V2P(rx_descs);

  // RDLEN
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RDLEN);
  *p = (uint32_t)sizeof(rx_descs);

  // RDH
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RDH);
  *p = 0;

  // RDT
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RDT);
  *p = PCI_E1000_NUM_RX_DESC - 1;

  // RCTL
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RCTL);
  *p = (1 << 1) | ((0 << 16) | (0 << 17)) | (1 << 26); // EN, BSIZE, and SECRC

  // transmit

  // TDBAL
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TDBAL);
  *p = (uint32_t)V2P(tx_descs);

  // TDLEN
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TDLEN);
  *p = (uint32_t)sizeof(tx_descs);

  // TDH
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TDH);
  *p = 0x0;

  // TDT
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TDT);
  *p = 0x0;

  // TCTL
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TCTL);
  *p = ((*p) & 0xffc00000) | (1 << 1) | (1 << 3) | (1 << 8) |
       (1 << 18); // EN, PSP, CT and COLD

  // TIPG
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TIPG);
  *p = ((*p) & 0xfffffc00) | 0xa; // IPGT

  // transmit_packet("foo", 3);

  return 1;
}

/*
 * ref. PCI/PCI-X Family of Gigabit Ethernet Controllers SDM 3.2
 *
 * Store packet length in len if it is not null.
 */
int receive_packet(char *buf, uint16_t *len) {
  struct rx_desc *const begin = (struct rx_desc *)P2V(
      (uintptr_t)PCI_E1000_REG_VALUE(PCI_E1000_REG_OFFSET_RDBAL));
  struct rx_desc *const end = begin + PCI_E1000_NUM_RX_DESC;
  struct rx_desc *tail = begin + PCI_E1000_REG_VALUE(PCI_E1000_REG_OFFSET_RDT);

  uint32_t *p;
  tail++;
  if (tail == end) {
    tail = begin;
  }

  if (!(tail->status & RX_DESC_STATUS_DD)) {
    // not buffer to read
    return -1;
  }

  if (tail->errors) {
    cprintf("errors of receive descriptor shows something wrong: 0x%02\n",
            tail->errors);
  }

  memcpy((void *)buf, (void *)P2V(((uintptr_t)tail->addr)), tail->length);

  // update RDT
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_RDT);
  *p = (uint32_t)(tail - begin);

  // store len if required
  if (len != NULL) {
    *len = tail->length;
  }
  return 0;
}

/*
 * ref. PCI/PCI-X Family of Gigabit Ethernet Controllers SDM 3.3.3 and 3.4
 */
int transmit_packet(char *data, uint16_t len) {
  cprintf("trasmit_packet: len: %d\n", len);
  struct tx_desc *const begin = (struct tx_desc *)P2V(
      (uintptr_t)PCI_E1000_REG_VALUE(PCI_E1000_REG_OFFSET_TDBAL));
  struct tx_desc *const end = begin + PCI_E1000_NUM_TX_DESC;
  struct tx_desc *tail = begin + PCI_E1000_REG_VALUE(PCI_E1000_REG_OFFSET_TDT);

  if ((tail->cmd & TX_DESC_CMD_RS) && !(tail->status & TX_DESC_STATUS_DD)) {
    // ring buffer is full
    return ERR_BUF;
  }

  tail->addr = (uint64_t)((uint32_t)V2P(tx_buf[tail - begin]));
  memcpy(tx_buf[tail - begin], data, len);
  tail->length = len;
  tail->cso = 0;
  tail->cmd = TX_DESC_CMD_EOP | TX_DESC_CMD_RS;
  tail->status = 0;
  tail->css = 0;
  tail->special = 0;

  char *x = (char *)P2V((uintptr_t)tail->addr);

  // update TDT;
  uint32_t *p;
  tail++;
  if (tail == end) {
    tail = begin;
  }
  p = PCI_E1000_REG_ADDR(PCI_E1000_REG_OFFSET_TDT);
  *p = (uint32_t)(tail - begin);

  return 0;
}
