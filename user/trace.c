#include "kernel/types.h"
#include "user/user.h"

int stoi(char *p) {
  int n = 0;
  while (*p) {
    n = n * 10 + *p - '0';
    ++p;
  }
  return n;
}

int main(int argc, char *argv[]) {
  if (argc <= 3) {
    fprintf(2, "Usage: trace mask cmd\n");
    exit(1);
  }
  int mask = stoi(argv[1]);
  // set mask in trace
  trace(mask);
  if (exec(argv[2], argv + 2) <= 0) {
    fprintf(2, "trace: failed to exec");
    for (int i = 2; i < argc; ++i) {
      fprintf(2, " %s", argv[i]);
    }
    fprintf(2, "\n");
    exit(1);
  }
  exit(0);
}