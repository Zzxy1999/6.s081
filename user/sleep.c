#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(2, "Usage: sleep time\n");
    exit(1);
  }
  uint32 time = atoi(argv[1]);
  if (sleep(time) < 0) {
    fprintf(2, "sleep %s: sleep failed\n", argv[1]);
  }
  exit(0);
}