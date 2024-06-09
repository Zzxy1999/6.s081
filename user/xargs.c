#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  /* 子进程的argc和argv直接继承一部分 */
  int argc1 = 0;
  char *argv1[MAXARG];
  while (argc1 + 1 < argc) {
    argv1[argc1] = argv[argc1 + 1];
    ++argc1;
  }
  char buf[512];
  int status;
  /* stdin重定向到管道左侧，之间读取 */
  while (read(0, buf, 512) > 0) {
    /* 按行划分参数，依次执行 */
    char *p = buf, *q = buf;
    while (p < buf + 512 && *p != '\0') {
      while (q < buf + 512 && *q != '\n' && *q != '\0') {
        ++q;
      } 
      *q = '\0';
      argv1[argc1] = p;
      ++q;
      p = q;
      if (fork() == 0) {
        exec(argv1[0], argv1);
      } else {
        wait(&status);
      }
    }
    memset(buf, 0, 512);
  }
  exit(0);
}