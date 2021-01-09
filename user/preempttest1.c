#include "user.h"

char *msg = "I am preemptiontest1\n";

int main(int argc, char *argv[]) {
  int pid;
  for (int i = 0; i < 100; i++) {
    printf("%s", msg);
  }
  printf("preemptiontest%d finished\n", 1);

  printf("argc and argv check\n");
  printf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  exit();
}
