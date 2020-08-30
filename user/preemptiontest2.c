#include "user.h"

char *msg = "I am preemptiontest2\n";

int main(void) {
  int pid;
  for (int i = 0; i < 100; i++) {
    print(msg);
  }
  exit();
}
