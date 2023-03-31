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




