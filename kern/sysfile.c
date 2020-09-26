#include "defs.h"
#include "fcntl.h"
#include "file.h"
#include "param.h"
#include "proc.h"
#include "stat.h"
#include "types.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int argfd(int n, int *pfd, struct file **pf) {
  int fd;
  struct file *f;

  if (argint(n, &fd) < 0) {
    return -1;
  }
  if (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0) {
    return -1;
  }
  if (pfd) {
    *pfd = fd;
  }
  if (pf) {
    *pf = f;
  }
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int fdalloc(struct file *f) {
  int fd;
  struct proc *curproc = myproc();

  for (fd = 0; fd < NOFILE; fd++) {
    if (curproc->ofile[fd] == 0) {
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int64_t sys_close(void) {
  int fd;
  struct file *f;

  if (argfd(0, &fd, &f) < 0)
    return -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

static struct inode *create(char *path, short type, short major, short minor) {
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if ((dp = nameiparent(path, name)) == 0) {
    return 0;
  }
  ilock(dp);

  if ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    if (type == T_FILE && ip->type == T_FILE) {
      return ip;
    }
    iunlockput(ip);
    return 0;
  }

  if ((ip = ialloc(dp->dev, type)) == 0) {
    panic("create: ialloc");
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if (type == T_DIR) { // Create . and .. entries.
    dp->nlink++;       // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0) {
      panic("create dots");
    }
  }

  if (dirlink(dp, name, ip->inum) < 0) {
    panic("create: dirlink");
  }

  iunlockput(dp);

  return ip;
}

int64_t sys_open(void) {
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  if (argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();

  if (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    if (ip == 0) {
      end_op();
      return -1;
    }
  } else {
    if ((ip = namei(path)) == 0) {
      end_op();
      return -1;
    }
    ilock(ip);
    if (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      return -1;
    }
  }

  if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    if (f) {
      fileclose(f);
    }
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  end_op();

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

int64_t sys_exec(void) {
  char *path, *argv[MAXARG];
  int i;
  uint64_t uargv, uarg;

  if (argstr(0, &path) < 0 || arg(1, &uargv) < 0) {
    return -1;
  }

  memset(argv, 0, sizeof(argv));

  for (i = 0;; i++) {
    if (i >= MAXARG) {
      return -1;
    }
    if (fetchint(uargv + sizeof(uintptr_t) * i, (uint64_t *)&uarg) < 0) {
      return -1;
    }
    if (uarg == 0) {
      argv[i] = 0;
      break;
    }
    if (fetchstr(uarg, &argv[i]) < 0) {
      return -1;
    }
  }

  return exec(path, argv);
}
