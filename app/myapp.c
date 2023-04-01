#include "myapp.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  void *end = sbrk(0);
  printf("addr of program break is %p\n", end);

  int x = 0xabcdef11;
  printf("0xabcdef11 & ~0x7 = %x\n", x & ~0x7);

  return 0;
}
