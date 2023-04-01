#include "memlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static char *mem_heap;     /* points to the first byte of heap */
static char *mem_brk;      /* points to the last byte of heap plus 1 */
static char *mem_max_addr; /* max legal heap addr plus 1 */

/**
 * @brief init heap and set brk and max heap addr
 *
 * @return 0 if success, -1 otherwise
 */
int mem_init() {
  if ((mem_heap = (char *)malloc(MAX_HEAP)) == NULL) {
    fprintf(stderr, "Error: mm_init failed, no memory available to allocate\n");
    return -1;
  };
  mem_brk = (char *)mem_heap;
  mem_max_addr = (char *)(mem_heap + MAX_HEAP);
  return 0;
}

/*!
 * @brief mm_sbrk increases heap by size of 'incr' bytes
 *
 * return old address of brk
 */
void *mem_sbrk(int incr) {
  char *old_brk = mem_brk;

  if ((incr < 0) || ((mem_brk + incr) >= mem_max_addr)) {
    errno = ENOMEM;
    fprintf(stderr, "Error: mm_sbrk failed. Ran out of memory ...\n");
    return (void *)-1l;
  }

  mem_brk += incr;
  return (void *)old_brk;
}

/*!
 * @brief mem_teardown tears down the allocated storage
 */
void mem_teardown() {
  free(mem_heap);
}
