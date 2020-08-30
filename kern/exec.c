#include "defs.h"
#include "elf.h"
#include "mmu.h"
#include "proc.h"

int exec(char *path, char **argv) {
  int i;
  uint64_t off;
  size_t sz;
  uintptr_t sp;
  struct elfhdr elf;
  struct proghdr ph;
  pte_t *pgdir = NULL, *oldpgdir;
  struct proc *curproc = myproc();
  size_t old_sz = curproc->sz;

  // TODO remote later (after fs)
  struct elfhdr *p_elf;
  struct proghdr *p_ph;

  // TODO for fs
  // begin_op();

  // TODO for fs
  // if((ip = namei(path)) == 0){
  //   end_op();
  //   cprintf("exec: fail\n");
  //   return -1;
  // }
  // ilock(ip);

  // Check ELF header
  // TODO for fs
  // if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf)) {
  //   goto bad;
  // }
  // Check ELF header
  // TODO remove later (after fs)
  if (strncmp(path, "/init", 15) == 0) {
    extern char _binary_obj_user_init_start[];
    p_elf = (struct elfhdr *)_binary_obj_user_init_start;
    elf = *p_elf;
  } else if (strncmp(path, "preemptiontest1", 15) == 0) {
    extern char _binary_obj_user_preemptiontest1_start[];
    p_elf = (struct elfhdr *)_binary_obj_user_preemptiontest1_start;
    elf = *p_elf;
  } else if (strncmp(path, "preemptiontest2", 15) == 0) {
    extern char _binary_obj_user_preemptiontest2_start[];
    p_elf = (struct elfhdr *)_binary_obj_user_preemptiontest2_start;
    elf = *p_elf;
  } else {
    goto bad;
  }

  if (elf.magic != ELF_MAGIC) {
    goto bad;
  }

  if ((pgdir = setupkvm()) == NULL) {
    goto bad;
  }

  // Load program into memory.
  sz = 0;
  for (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
    // TODO for fs
    // if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph)) {
    //   goto bad;
    // }
    // TODO remove later (after fs)
    p_ph = (struct proghdr *)(((char *)p_elf) + off);
    ph = *p_ph;

    if (ph.type != ELF_PROG_LOAD) {
      continue;
    }
    if (ph.memsz < ph.filesz) {
      goto bad;
    }
    if (ph.vaddr + ph.memsz < ph.vaddr) {
      goto bad;
    }
    if ((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0) {
      goto bad;
    }
    if (ph.vaddr % PGSIZE != 0) {
      goto bad;
    }
    // TODO for fs
    // if (loaduvm(pgdir, (char *)ph.vaddr, ip, ph.off, ph.filesz) < 0) {
    //   goto bad;
    // }
    // TODO remove later (after fs)
    if (loaduvm(pgdir, (char *)ph.vaddr, (char *)p_elf, ph.off, ph.filesz) <
        0) {
      goto bad;
    }
  }
  // TODO for fs
  // iunlockput(ip);
  // end_op();
  // ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if ((sz = allocuvm(pgdir, sz, sz + 2 * PGSIZE)) == 0) {
    goto bad;
  }
  clearpteu(pgdir, (char *)(sz - 2 * PGSIZE));
  sp = sz;

  // TODO: handle exec arguments
  // Push argument strings, prepare rest of stack in ustack.
  // for(argc = 0; argv[argc]; argc++) {
  //   if(argc >= MAXARG)
  //     goto bad;
  //   sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
  //   if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
  //     goto bad;
  //   ustack[3+argc] = sp;
  // }
  // ustack[3+argc] = 0;

  // ustack[0] = 0xffffffff;  // fake return PC
  // ustack[1] = argc;
  // ustack[2] = sp - (argc+1)*4;  // argv pointer

  // sp -= (3+argc+1) * 4;
  // if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
  //   goto bad;

  // Save program name for debugging.
  char *last, *s;
  for (last = s = path; *s; s++) {
    if (*s == '/') {
      last = s + 1;
    }
  }
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->rip = elf.entry; // main
  curproc->tf->rsp = sp;
  switchuvm(curproc);
  freevm(oldpgdir, old_sz);
  return 0;

bad:
  if (pgdir) {
    freevm(pgdir, sz);
  }
  // TODO for fs
  // if(ip){
  //   iunlockput(ip);
  //   end_op();
  // }
  return -1;
}
