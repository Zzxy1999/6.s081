#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"
#include "elf.h"

void put_mem(struct largemem_t *lm, char *buf) {
  if (lm->cnt == MAX_LARGE_MEM) {
    panic("put_mem: oom");
  }
  lm->mem[lm->cnt++] = buf;
}

void free_mem(struct largemem_t *lm) {
  for (int i = 0; i < lm->cnt; ++i) {
    kfree(lm->mem[i]);
  }
}

void print_mem(struct largemem_t *lm) {

}

uint64 analyse_elf(char *path, char *sec_name) {
  
  struct inode *ip;
  struct elfhdr elf;
  char *table = 0;
  struct largemem_t lm;
  begin_op();

  if ((ip = namei(path)) == 0) {
    end_op();
    return -1;
  }
  ilock(ip);

  if(readi(ip, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf)) {
    printf("analyse_elf: elf\n");
    goto bad;
  }
  
  if (elf.magic != ELF_MAGIC) {
    printf("analyse_elf:  magic\n");
    goto bad;
  }

  shdr sechdr;
  uint64 off;

  off = elf.shoff + elf.shstrndx * sizeof(sechdr);
  // strtab
  if (readi(ip, 0, (uint64)&sechdr, off, sizeof(sechdr)) != sizeof(sechdr)) {
    printf("analyse_elf: strtab sec\n");
    goto bad;
  }

  table = kalloc();
  memset(table, 0, sizeof(table));
  if (readi(ip, 0, (uint64)table, sechdr.off, sechdr.size) != sechdr.size) {
    printf("analyse_elf: strtab table\n");
    goto bad;
  }

  // print all section
  printf("name\t\ttype\t\toff\t\tsize\n");
  for (int i = 1; i < elf.shnum; ++i) {
    off = elf.shoff + i * sizeof(sechdr);
    if (readi(ip, 0, (uint64)&sechdr, off, sizeof(sechdr)) != sizeof(sechdr)) {
      printf("analyse_elf: sechdr\n");
      goto bad;
    }
    // hdr info
    printf("%s\t", table + sechdr.name);
    if (strlen(table + sechdr.name) < 8) {
      printf("\t");
    }
    printf("%d\t\t%d\t\t%d\n", sechdr.type, sechdr.off, sechdr.size);

    if (!strncmp(table + sechdr.name, sec_name, sizeof(table + sechdr.name))) {
      uint64 n = 0;
      while (n < sechdr.size) {
        char *section = kalloc();
        if (!section) {
          printf("analyse_elf: kalloc\n");
          goto bad;
        }
        if (readi(ip, 0, (uint64)section, sechdr.off, sechdr.size) != sechdr.size) {
          printf("analyse_elf: read section\n");
          continue;
        }
        lm.mem[lm.cnt++] = section;
      }
    }
  }
  iunlockput(ip);
  end_op();
  kfree(table);
  free_mem(&lm);
  return 0;

bad:
  if (ip) {
    iunlockput(ip);
    end_op();
  }
  if (table) {
    kfree(table);
  }
  free_mem(&lm);
  return -1;
}