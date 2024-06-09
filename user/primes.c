#include "kernel/types.h"
#include "user/user.h"

/* 测试最多到227 */
#define MAXNUM 35

int status;

/* p用于main调用Sieve时向第一个筛传递数字 */
void Sieve(int p[]) {
  /* 当前进程读到的质数、数 */
  int pnum, num;
  int pleft[2], pright[2];
  /* 初次调用的初始化 */
  pright[0] = p[0];
  pright[1] = p[1];
  while (1) {
    /* pleft和pright都是父进程的拷贝，父亲的pright是自己的left */
    pleft[0] = pright[0];
    pleft[1] = pright[1];
    /* 没有质数，退出循环，不再创建新的进程 */
    if (read(pleft[0], &pnum, sizeof(pnum)) <= 0) {
      close(pleft[0]);
      break;
    }
    /* 读第一个数为质数 */
    printf("prime %d\n", pnum);
    /* 与子进程的通信 */
    if (pipe(pright) < 0) {
      fprintf(2, "primes: pipe failed\n");
      exit(1);
    }
    /* 创建子进程作为右边的筛 */
    if (fork() == 0) {
      /* 子进程关闭不需要的描述符后进入下一次循环后开始左读右写 */
      close(pleft[0]);
      close(pright[1]);
    } else {
      close(pright[0]);
      /* 左读右写 */
      while (read(pleft[0], &num, sizeof(num)) > 0) {
        if (num % pnum != 0) {
          write(pright[1], &num, sizeof(num));
        }
      }
      close(pleft[0]);
      close(pright[1]);
      wait(&status);
    }
  }
}

int main(int argc, char *argv[]) {
  int p[2];
  if (pipe(p) < 0) {
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }
  /* 向第一个筛传递2 ～ MAXNUM数字 */
  if (fork() == 0) {
    close(p[1]);
    Sieve(p);
    close(p[0]);
  } else {
    close(p[0]);
    for (int i = 2; i <= MAXNUM; ++i) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait(&status);
  }
  exit(0);
}