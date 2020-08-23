// The local APIC manages internal (non-I/O) interrupts.
// See Chapter 8 and 10 of Intel SDM vol.3

#include "trap.h"
#include "x86.h"

// Local APIC registers, divided by 4 for use as uint[] indices.
#define ID (0x0020 / 4)               // ID
#define VER (0x0030 / 4)              // Version
#define TPR (0x0080 / 4)              // Task Priority
#define EOI (0x00B0 / 4)              // EOI
#define SVR (0x00F0 / 4)              // Spurious Interrupt Vector
#define SVR_ENABLE 0x00000100         // Unit Enable
#define ESR (0x0280 / 4)              // Error Status
#define ICRLO (0x0300 / 4)            // Interrupt Command
#define ICR_DELIVM_INIT 0x00000500    // Delivery Mode: INIT/RESET
#define ICR_DELIVM_STARTUP 0x00000600 // Delivery Mode: Startup IPI
#define ICR_DELIVS 0x00001000         // Delivery status
#define ICR_LEVEL_ASSERT 0x00004000   // Level: Assert interrupt
#define ICR_TRIGGER_LEVEL 0x00008000  // Trigger Mode: Level
#define ICR_DEST_BCAST                                                         \
  0x00080000                // Destination: Send to all APICs, including self.
#define ICR_BUSY 0x00001000 // busy if this bit on? this is same as ICR_DELVIS
#define ICRHI (0x0310 / 4)  // Interrupt Command [63:32]
#define TIMER (0x0320 / 4)  // Local Vector Table 0 (TIMER)
#define TIMER_PERIODIC 0x00020000 // Periodic
#define PCINT (0x0340 / 4)        // Performance Counter LVT
#define LINT0 (0x0350 / 4)        // Local Vector Table 1 (LINT0)
#define LINT1 (0x0360 / 4)        // Local Vector Table 2 (LINT1)
#define ERROR (0x0370 / 4)        // Local Vector Table 3 (ERROR)
#define LVT_MASKED 0x00010000     // Interrupt masked
#define TICR (0x0380 / 4)         // Timer Initial Count
#define TCCR (0x0390 / 4)         // Timer Current Count
#define TDCR (0x03E0 / 4)         // Timer Divide Configuration
#define TDCR_X1 0x0000000B        // divide counts by 1

volatile uint32_t *lapic; // Initialized in mp.c

static void lapicw(int index, uint32_t value) {
  lapic[index] = value;
  lapic[ID]; // wait for write to finish, by reading
}

// Clear error status register (requires back-to-back writes).
// See Intel SDM vol.3 10.5.3 Error Handling
static void reset_esr(void) {
  // First write is to clear register.
  lapicw(ESR, 0);
  // According to the manual, local APIC might update the register
  // based on an error detected since the last write to the ESR.
  // It means one error might exist at most, so the second write
  // is required and enough to reset ESR?
  lapicw(ESR, 0);
}

// Ack any outstanding interrupts.
static void eoi(void) { lapicw(EOI, 0); }

void lapicinit(void) {
  if (!lapic) {
    return;
  }

  // Enable local APIC; set spurious interrupt vector.
  //
  // Set SVR_ENABLE of SVR is one way of enabling local APIC according to Intel
  // SVM 10.4.3. I'm not sure what spurious interrupt is, but it is something
  // like unexpected interrupt?
  lapicw(SVR, SVR_ENABLE | (T_IRQ0 + IRQ_SPURIOUS));

  // The timer repeatedly counts down at bus frequency
  // from lapic[TICR] and then issues an interrupt.
  // If xv6 cared more about precise timekeeping,
  // TICR would be calibrated using an external time source.
  //
  // See Intel SDM Vol3 10.5.4 APIC Timer
  lapicw(TDCR, TDCR_X1);
  lapicw(TIMER, TIMER_PERIODIC | (T_IRQ0 + IRQ_TIMER));
  lapicw(TICR, 10000000);

  // Disable logical interrupt lines.
  lapicw(LINT0, LVT_MASKED);
  lapicw(LINT1, LVT_MASKED);

  // Disable performance counter overflow interrupts
  // on machines that provide that interrupt entry.
  //
  // According to Intel SDM vol.3 10.4.8 Local APIC Version Register,
  // the value returned is 4 for the P6 family processors (which have 5 LVT
  // entries).
  if (((lapic[VER] >> 16) & 0xFF) >= 4) {
    lapicw(PCINT, LVT_MASKED);
  }

  // Map error interrupt to IRQ_ERROR.
  lapicw(ERROR, T_IRQ0 + IRQ_ERROR);

  // Clear error status register (requires back-to-back writes).
  reset_esr();

  // Ack any outstanding interrupts.
  eoi();

  // Send an Init Level De-Assert to synchronise arbitration ID's.
  lapicw(ICRHI, 0);
  lapicw(ICRLO, ICR_DEST_BCAST | ICR_DELIVM_INIT | ICR_TRIGGER_LEVEL);
  while (lapic[ICRLO] & ICR_DELIVS) {
    ;
  }

  // Enable interrupts on the APIC (but not on the processor).
  lapicw(TPR, 0);
}

int lapicid(void) {
  if (!lapic)
    return 0;
  return lapic[ID] >> 24;
}

// Stupid I/O delay routine necessitated by historical PC design flaws
void microdelay(int us) {
  inb(0x84);
  inb(0x84);
  inb(0x84);
  inb(0x84);
}
