// Multiprocessor support
// Search memory for MP description structures.
// http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include "mp.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"

struct cpu cpus[NCPU];

static uint8_t sum(uint8_t *addr, int len) {
  int i, sum;

  sum = 0;
  for (i = 0; i < len; i++)
    sum += addr[i];
  return sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp *mpsearch1(uintptr_t a, int len) {
  uint8_t *e, *p, *addr;

  addr = (uint8_t *)P2V(a);
  e = addr + len;
  for (p = addr; p < e; p += sizeof(struct mp))
    if (memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
      return (struct mp *)p;
  return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp *mpsearch(void) {
  uchar *bda;
  uint p;
  struct mp *mp;

  bda = (uchar *)P2V(0x400);
  if ((p = ((bda[0x0F] << 8) | bda[0x0E]) << 4)) {
    if ((mp = mpsearch1(p, 1024)))
      return mp;
  } else {
    p = ((bda[0x14] << 8) | bda[0x13]) * 1024;
    if ((mp = mpsearch1(p - 1024, 1024)))
      return mp;
  }
  return mpsearch1(0xF0000, 0x10000);
}

// Search for an MP configuration table.  For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
//
// ref. MPspec Chapter 4
static struct mpconf *mpconfig(struct mp **pmp) {
  struct mpconf *conf;
  struct mp *mp;

  if ((mp = mpsearch()) == 0 || mp->physaddr == 0)
    return 0;
  conf = (struct mpconf *)P2V((uintptr_t)mp->physaddr);
  if (memcmp(conf, "PCMP", 4) != 0)
    return 0;
  if (conf->version != 1 && conf->version != 4)
    return 0;
  if (sum((uchar *)conf, conf->length) != 0)
    return 0;
  *pmp = mp;
  return conf;
}

void mpinit(void) {
  uchar *p, *e;
  int ismp;
  struct mp *mp;
  struct mpconf *conf;

  if ((conf = mpconfig(&mp)) == 0) {
    panic("Expect to run on an SMP");
  }

  ismp = 1;

  cprintf("mp: 0x%p, mpconf: 0x%p\n", mp, conf);
}
