#include "user.h"

char *msg = "I am preemptiontest1\n";

int main(void) {
  int pid;

  if ((pid = fork()) < 0) {
    print("An error occurred in fork.\n");
    return 1;
  } else if (pid == 0) {
    // child
    exec("preemptiontest2", NULL);
  } else {
    // parent
    while (1) {
      print(msg);
    }
  }
}
