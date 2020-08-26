#include "user.h"

char *msg = "I am preemptiontest1\n";

int main(void) {
  while (1) {
    print(msg);
  }
}
