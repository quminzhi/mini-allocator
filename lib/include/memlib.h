#ifndef MEM_LIB_H
#define MEM_LIB_H

#define MAX_HEAP (1 << 24)   /* Set the maximum capacity of heap to be 8MB */

int mem_init();
void *mem_sbrk(int incr);
void mem_teardown();

#endif /* MEM_LIB_H */
