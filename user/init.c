#include "user.h"

char *argv[] = {"sh", 0};

int main(void) {
  pid_t pid, wpid;

  if (open("console", O_RDWR) < 0) {
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  // process for network
  if ((pid = fork()) < 0) {
    printf("init: fork failed\n");
    exit();
  } else if (pid == 0) {
    tcpip_worker();
  }

  for (;;) {
    printf("init: starting sh\n");
    pid = fork();
    if (pid < 0) {
      printf("init: fork failed\n");
      exit();
    }
    if (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit();
    }
    while ((wpid = wait()) >= 0 && wpid != pid) {
      printf("zombie!\n");
    }
  }

  // pid_t pid1, pid2, pid3;
  // pid1 = fork();
  // if (pid1 < 0) {
  //   printf("init: 1st fork failed\n");
  //   exit();
  // }
  // if (pid1 == 0) {
  //   // child
  //   char *argv[] = {"preemptiontest1", "second arg", NULL};
  //   exec(argv[0], argv);
  //   printf("init: failed to exec preemptiontest1\n");
  //   exit();
  // }

  // pid2 = fork();
  // if (pid2 < 0) {
  //   printf("init: 2nd fork failed\n");
  //   exit();
  // }
  // if (pid2 == 0) {
  //   // child
  //   char *argv[] = {"preemptiontest2", NULL};
  //   exec(argv[0], argv);
  //   printf("init: failed to exec preemptiontest2\n");
  //   exit();
  // }

  // int ok1 = 0;
  // int ok2 = 0;
  // for (int i = 0; i < 2; i++) {
  //   pid_t pid = wait();
  //   if (pid == pid1) {
  //     ok1 = 1;
  //   } else if (pid == pid2) {
  //     ok2 = 1;
  //   }
  // }

  // if (ok1 && ok2) {
  //   printf("init: preemptiontest success\n");
  // } else {
  //   printf("init: preemptiontest fail\n");
  // }

  // printf("fstest started\n");
  // pid3 = fork();
  // if (pid3 < 0) {
  //   printf("init: 3rd fork failed\n");
  //   exit();
  // }
  // if (pid3 == 0) {
  //   // child
  //   char *argv[] = {"fstest", NULL};
  //   exec(argv[0], argv);
  //   printf("init: failed to exec fstest\n");
  //   exit();
  // }
  // wait();
  // printf("fstest finished\n");

  // for (;;) {
  // }
}
