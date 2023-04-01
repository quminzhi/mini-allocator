# Mini Allocator

## Overview

Mini allocator is a dynamic memory allocator which implements different
data structures, algorithms, and splitting and coalescing policies. It can be
used to test the performance of various design pattern for dynamic storage
allocator.

## Architecture

Dynamic storage allocator will consist of the following four functions, which
are declared in `mm.h` and defined in `mm.c`.

```c
int mm_init(void );
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
```

- `mm_init`: performs any necessary initializations, such as allocating the 
initial heap area. The return value should be -1 if there was a problem in 
performing the initialization, 0 otherwise.
- `mm_malloc`: returns a pointer to an allocated block payload of at least 
size bytes (8-byte aligned pointer). The entire allocated block should lie 
within the heap region and should not overlap with any other allocated chunk.
- `mm_free`: frees the block pointed to by ptr. It returns nothing. This 
routine is only guaranteed to work when the passed pointer (ptr) was returned 
by an earlier call to `mm_malloc` or `mm_realloc` and has not yet been freed.
- `mm_realloc`: returns a pointer to an allocated region of at least size 
bytes with the following constraints.
  - if ptr is NULL, the call is equivalent to `mm_malloc(size)`;
  - if size is equal to zero, the call is equivalent to `mm_free(ptr)`;
  - if ptr is not NULL, it must have been returned by an earlier call to 
  `mm_malloc` or `mm_realloc`.
  - The contents of the new block are the same as those of the old ptr block,
  up to the minimum of the old and new sizes. Everything else is uninitialized.

### Initialization

At first, we asked memory to allocate a space with sizeof `MAX_HEAP` which is
defined in `memlib.h` as 8MB and set up `mem_heap`, which points to the start
address of heap, `mem_brk`, points to the top (excluded) of the heap, and
`mem_max_addr`, which points to the first invalid address of heap space (or say
max legal heap space addr plus 1).

`mem_init` is included in `memlib` library, which also provides `mem_sbrk`, used
to move up and down `mem_brk` to reflect the size of heap, and `mem_teardown`,
freeing allocated space for the heap.

The manipulation routines are in `mm` library, which are discussed in the last
section. For more detail, `mm_init` initialize the heap with following start
setups.

```c
// start setups:
//
//  |<----------  heap space  -------------->|
//
//              * heap_listp              
//  +-----+-----+-----+-----+----------------+
//  | pad | hdr | ftr | hdr |      ....      |
//  +-----+-----+-----+-----+----------------+
//        | prologue  | epi |                
//  ^                       ^                ^
// mem_heap              mem_brk        heap_max_addr
```

Word-size epilogue at the end of the heap is a tricky setup, which makes split
and coalesce unified by providing a "placeholder" for the header of a future
block. (resolve edge cases cleverly)

`heap_listp` points to the middle part of prologue is also a careful design for
accessing header and tail of some block in a unified way.

### Splitting and Coalescing

Splitting occurs when we allocate storage in-place. Do not forget to take into
consideration the size of footer and header that comes with new blocks. `2 *
DSIZE` in routine `place` is the minimum payload of a block must be greater than
0 and the alignment requirement (`DSIZE` alignment), which means the sum of
minimum payload size (`DSIZE`) and size of header and footer (`DSIZE`).

```c
static void place(char *bp, size_t new_size) {
  size_t old_size = GET_SIZE(HDRP(bp));

  if ((old_size - new_size) >= (2 * DSIZE)) {
    // split
    PUT(HDRP(bp), PACK(new_size, 1));
    PUT(FTRP(bp), PACK(new_size, 1));
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(old_size - new_size, 0));
    PUT(FTRP(bp), PACK(old_size - new_size, 0));
  } else {
    // no need to split
    PUT(HDRP(bp), PACK(new_size, 1));
    PUT(FTRP(bp), PACK(new_size, 1));
  }
}
```

Coalescing is considered in four senarioes.

```c
/*!
 * @brief coalesce adjacent blocks to current block if possible
 *
 * case 1: both previous and next blocks are allocated
 * case 2: previous one allocated and next one freed
 * case 3: previous one freed and next one allocated
 * case 4: both of them are freed
 *
 * return bp
 */
static void *coalesce(char *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) {
    return bp;
  } else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp); /* !!! */
  } else {
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
  return bp;
}
```

## Contribution

This project is supervised under Prof. Wes.

Contact: quminzhi@gmail.com
