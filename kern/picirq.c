// ref. https://wiki.osdev.org/8259_PIC
// ref. https://pdos.csail.mit.edu/6.828/2018/readings/hardware/8259A.pdf

#include "x86.h"

// I/O Addresses of the two programmable interrupt controllers
#define PIC_MASTER_COMMAND 0x20 // Master - Command (IRQs 0-7)
#define PIC_MASTER_DATA 0x21    // Master - Data (IRQs 0-7)
#define PIC_SLAVE_COMMAND 0xA0  // Slave - Command (IRQs 8-15)
#define PIC_SLAVE_DATA 0xA1     // Slave - Data (IRQs 8-15)

// Don't use the 8259A interrupt controllers.  Xv6 assumes SMP hardware.
void picinit(void) {
  // mask all interrupts
  outb(PIC_MASTER_DATA, 0xFF);
  outb(PIC_SLAVE_DATA, 0xFF);
}
