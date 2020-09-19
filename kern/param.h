#ifndef XV6_X86_64_PARAM_H
#define XV6_X86_64_PARAM_H

#define NPROC 64                  // maximum number of processes
#define KSTACKSIZE 4096           // size of per-process kernel stack
#define NCPU 8                    // maximum number of CPUs
#define MAXARG 32                 // max exec arguments
#define MAXOPBLOCKS 10            // max # of blocks any FS op writes
#define LOGSIZE (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS * 3)    // size of disk block cache
#define FSSIZE 1000               // size of file system in blocks

#endif /* XV6_X86_64_PARAM_H */
