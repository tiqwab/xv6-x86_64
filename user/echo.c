#include "user.h"

void echo(int n, char *msgs[]) {
  for (int i = 0; i < n; i++) {
    dprintf(1, "%s%s", msgs[i], i + 1 < n ? " " : "\n");
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    dprintf(2, "usage: %s <string>...\n", argv[0]);
    exit();
  }

  echo(argc - 1, &argv[1]);
  exit();
}
