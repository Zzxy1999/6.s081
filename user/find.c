#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/* 由path找到当前的name */
char *Fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;
  
  if(strlen(p) >= DIRSIZ) {
    return p;
  }

  memmove(buf, p, strlen(p));
  //填充'\0'
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
  return buf;
}

void Find(char* path, char* name) {
  int fd;
  char buf[512], *p, *fname;
  struct stat st;
  struct dirent de;

  memset(buf, 0, 512);
  
  fname = Fmtname(path);

  /* 找到 */
  if (strcmp(fname, name) == 0) {
    printf("%s\n", path);
    /* 可能当前是name目录，而目录内部有name文件，所以不能return */
    //return;
  }
  
  if ((fd = open(path, 0)) < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    printf("find: cannot stat %s\n", path);
    return;
  }

  /* 目录内部找 */
  if (st.type == T_DIR) {
    memcpy(buf, path, strlen(path));
    while (read(fd, &de, sizeof(de)) == sizeof(de) && de.inum != 0) {
      p = buf + strlen(path);
      *p++ = '/';
      /* 目录的前两个de分别是.和.. 不要递归*/
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      /* 递归 */
      Find(buf, name);
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find pathname filename\n");
    exit(1);
  }
  Find(argv[1], argv[2]);
  exit(0);
}