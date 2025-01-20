#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    printf("Usage: elftest target\n");
    return -1;
  }
  if (argc == 2) {
    elf(argv[1], "");
  } else {
    elf(argv[1], argv[2]);
  }
  
  return 0;
}

int add(int a, int b) {
  // cmt1
  // cmt2
  // cmt2
  int c = a + b;
  return c;
}

void helloworld() {
  printf("helloworld\n");
}