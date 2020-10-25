#include "user.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    dprintf(2, "Usage: %s <target> <link_name>\n");
    exit();
  }
  if (link(argv[1], argv[2]) < 0)
    dprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit();
}
