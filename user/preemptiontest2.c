#include "user.h"

char *msg = "I am preemptiontest2\n";

// almost same as preemptiontest1, but
// - msg is different
// - not call exit in main
int main(void) {
  int pid;
  for (int i = 0; i < 100; i++) {
    printf("%s", msg);
  }
  printf("preemptiontest%d finished\n", 2);
}
