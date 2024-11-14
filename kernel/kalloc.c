// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct {
  int cnt[(PHYSTOP - KERNBASE) / PGSIZE];
  struct spinlock lock;
} paref;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&paref.lock, "paref");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  int n = prdecr((uint64)pa);
  if (n > 0) {
    return;
  }
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP) {
    panic("kfree");
  }
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  prset((uint64)r, 1);
  return (void*)r;
}

void prlock() {
  acquire(&paref.lock);
}

void prunlock() {
  release(&paref.lock);
}

inline int prget(uint64 pa) {
  acquire(&paref.lock);
  int n = paref.cnt[PA2INDEX(pa)];
  release(&paref.lock);
  return n;
}

inline void prset(uint64 pa, int n) {
  acquire(&paref.lock);
  paref.cnt[PA2INDEX(pa)] = n;
  release(&paref.lock);
}

inline void princr(uint64 pa) {
  acquire(&paref.lock);
  paref.cnt[PA2INDEX(pa)]++;
  release(&paref.lock);
}

// must atomic
inline int prdecr(uint64 pa) {
  acquire(&paref.lock);
  int n = --paref.cnt[PA2INDEX(pa)];
  release(&paref.lock);
  return n;
}