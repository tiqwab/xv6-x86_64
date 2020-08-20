// ref. Intel Multiprocessor Specification (MPspec)
// https://pdos.csail.mit.edu/6.828/2014/readings/ia32/MPspec.pdf

#include "types.h"

// MPspec 4.1 MP Floating Pointer Structure
struct mp {
  char signature[4]; // "_MP_"
  uint32_t physaddr; // phys addr of MP config table
  uint8_t length;    // 1
  uint8_t specrev;   // [14]
  uint8_t checksum;  // all bytes must add up to 0
  uint8_t type;      // MP system config type
  uint8_t imcrp;
  uint8_t reserved[3];
};

// MPsepc 4.2 MP Configuration Table Header
struct mpconf {
  char signature[4];   // "PCMP"
  uint16_t length;     // total table length
  uint8_t version;     // [14]
  uint8_t checksum;    // all bytes must add up to 0
  uint8_t product[20]; // product id
  uint32_t oemtable;   // OEM table pointer
  uint16_t oemlength;  // OEM table length
  uint16_t entry;      // entry count
  uint32_t lapicaddr;  // address of local APIC
  uint16_t xlength;    // extended table length
  uint8_t xchecksum;   // extended table checksum
  uint8_t reserved;
};
