// Simple PIO-based (non-DMA) IDE driver code.
// ref. [OSDev](https://wiki.osdev.org/PCI_IDE_Controller)
// ref.
// [Spec](http://www.t13.org/Documents/UploadedDocuments/project/d0791r4c-ATA-1.pdf)
// ref. [About Compatibility Mode](http://www.bswd.com/pciide.pdf)

#include "buf.h"
#include "defs.h"
#include "proc.h"
#include "spinlock.h"
#include "trap.h"
#include "x86.h"

#define SECTOR_SIZE 512

// status
#define SR_BSY 0x80  // busy
#define SR_DRDY 0x40 // drive ready
#define SR_DWF 0x20  // drive write fault
#define SR_DRQ 0x08  // data request
#define SR_ERR 0x01  // error

// for sending command to drive or posting status from the drive
#define PRIMARY_COMMAND_BASE_REG 0x1f0
// for drive control and post alternate status
#define PRIMARY_CONTROL_BASE_REG 0x3f6

// REG + reg` is a target port
#define REG_DATA 0x00      // Read-Write
#define REG_ERROR 0x01     // Read Only
#define REG_FEATURES 0x01  // Write Only
#define REG_SECCOUNT0 0x02 // Read-Write
#define REG_LBA0 0x03      // Read-Write
#define REG_LBA1 0x04      // Read-Write
#define REG_LBA2 0x05      // Read-Write
#define REG_HDDEVSEL 0x6   // Read-Write, used to select a drive in the channel.
#define REG_COMMAND 0x07   // Write Only
#define REG_STATUS 0x07    // Read Only

// Command codes
// See 9 Command Description in Spec
#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_RDMUL 0xc4
#define IDE_CMD_WRMUL 0xc5

// idequeue points to the buf now being read/written to the disk.
// idequeue->qnext points to the next buf to be processed.
// You must hold idelock while manipulating queue.

static struct spinlock idelock;
static struct buf *idequeue;

static int havedisk1;

// Wait for IDE disk to become ready.
static int idewait(int checkerr) {
  int r;

  while (((r = inb(PRIMARY_COMMAND_BASE_REG + REG_STATUS)) &
          (SR_BSY | SR_DRDY)) != SR_DRDY) {
    ;
  }
  if (checkerr && (r & (SR_DWF | SR_ERR)) != 0) {
    return -1;
  }
  return 0;
}

void ideinit(void) {
  int i;

  initlock(&idelock, "ide");
  ioapicenable(IRQ_IDE, ncpu - 1);
  idewait(0);

  // switch to Device 1
  // ref. 7.2.8 Drive/head register in Spec
  outb(PRIMARY_COMMAND_BASE_REG + REG_HDDEVSEL, 0xe0 | (1 << 4));
  for (i = 0; i < 1000; i++) {
    int r = inb(PRIMARY_COMMAND_BASE_REG + REG_STATUS);
    if (r != 0) {
      if ((r & (SR_BSY | SR_DWF | SR_ERR)) == 0) {
        havedisk1 = 1;
        break;
      }
    }
  }

  // Switch back to disk 0.
  outb(PRIMARY_COMMAND_BASE_REG + REG_HDDEVSEL, 0xe0 | (0 << 4));
}

// Start the request for b.  Caller must hold idelock.
void idestart(struct buf *b) {
  if (b == 0) {
    panic("idestart");
  }
  if (b->blockno >= FSSIZE) {
    panic("incorrect blockno");
  }
  int sector_per_block = BSIZE / SECTOR_SIZE;
  int sector = b->blockno * sector_per_block;
  int read_cmd = (sector_per_block == 1) ? IDE_CMD_READ : IDE_CMD_RDMUL;
  int write_cmd = (sector_per_block == 1) ? IDE_CMD_WRITE : IDE_CMD_WRMUL;

  if (sector_per_block > 7) {
    panic("idestart");
  }

  idewait(0);

  // This is Device Control Register? (7.2.6 in Spec).
  // Generate interrupt
  outb(PRIMARY_CONTROL_BASE_REG, 0);

  // This register contains the number of sectors of data requested to be
  // transferred on a read or write operation between the host and the drive.
  // See 7.2 in Spec.
  outb(PRIMARY_COMMAND_BASE_REG + REG_SECCOUNT0, sector_per_block);

  // This register contains the starting sector number for any disk data access
  // for the subsequent command.
  // As we set up in `ide_init`, addressing is based on LBA not CHS.
  // See 7.2 in Spec.
  outb(PRIMARY_COMMAND_BASE_REG + REG_LBA0, sector & 0xff);
  outb(PRIMARY_COMMAND_BASE_REG + REG_LBA1, (sector >> 8) & 0xff);
  outb(PRIMARY_COMMAND_BASE_REG + REG_LBA2, (sector >> 16) & 0xff);

  outb(PRIMARY_COMMAND_BASE_REG + REG_HDDEVSEL,
       0xe0 | ((b->dev & 1) << 4) | ((sector >> 24) & 0x0f));

  if (b->flags & B_DIRTY) {
    // This register contains the command code being sent to the drive.
    // Command execution begins immediately after this register is written.
    //
    // The detail of write protocol is in 10.2 of Spec
    outb(PRIMARY_COMMAND_BASE_REG + REG_COMMAND, write_cmd);
    // TODO: does it work as expected?
    outsl(PRIMARY_COMMAND_BASE_REG + REG_DATA, b->data, BSIZE / 4);
  } else {
    // The detail of read protocol is in 10.1 of Spec
    outb(PRIMARY_COMMAND_BASE_REG + REG_COMMAND, read_cmd);
  }
}

// Interrupt handler.
void ideintr(void) {
  struct buf *b;

  // First queued buffer is the active request.
  acquire(&idelock);

  if ((b = idequeue) == 0) {
    release(&idelock);
    return;
  }
  idequeue = b->qnext;

  // Read data if needed.
  if (!(b->flags & B_DIRTY) && idewait(1) >= 0) {
    // TODO: does it work as expected?
    insl(PRIMARY_COMMAND_BASE_REG + REG_DATA, b->data, BSIZE / 4);
  }

  // Wake process waiting for this buf.
  b->flags |= B_VALID;
  b->flags &= ~B_DIRTY;
  wakeup(b);

  // Start disk on next buf in queue.
  if (idequeue != 0) {
    idestart(idequeue);
  }

  release(&idelock);
}

// Sync buf with disk.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void iderw(struct buf *b) {
  struct buf **pp;

  if (!holdingsleep(&b->lock)) {
    panic("iderw: buf not locked");
  }
  if ((b->flags & (B_VALID | B_DIRTY)) == B_VALID) {
    panic("iderw: nothing to do");
  }
  if (b->dev != 0 && !havedisk1) {
    panic("iderw: ide disk 1 not present");
  }

  acquire(&idelock);

  // Append b to idequeue.
  b->qnext = 0;
  for (pp = &idequeue; *pp; pp = &(*pp)->qnext) {
    ;
  }
  *pp = b;

  // Start disk if necessary.
  if (idequeue == b) {
    idestart(b);
  }

  // Wait for request to finish.
  while ((b->flags & (B_VALID | B_DIRTY)) != B_VALID) {
    sleep(b, &idelock);
  }

  release(&idelock);
}
