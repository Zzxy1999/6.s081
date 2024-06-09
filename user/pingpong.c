#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  /* p1: parent->child */
  /* p2: child->parent */
  int p1[2], p2[2];
  if (pipe(p1) < 0) {
    fprintf(2, "pingpong: pipe failed\n");
    exit(1);
  }
  if (pipe(p2) < 0) {
    fprintf(2, "pingpong: piepe failed\n");
    exit(1);
  }
  char buff[1];
  int pid = fork();
  if (pid == 0) {
    pid = getpid();
    /*child*/
    close(p1[1]);
    close(p2[0]);
    read(p1[0], buff, 1);
    close(p1[0]);
    printf("%d: received ping\n", pid);
    write(p2[1], buff, 1);
    close(p2[1]);
  } else {
    pid = getpid();
    /*parent*/
    close(p1[0]);
    close(p2[1]);
    write(p1[1], "P", 1);
    close(p1[1]);
    read(p2[0], buff, 1);
    close(p2[0]);
    printf("%d: received pong\n", pid);
  }
  exit(0);
}