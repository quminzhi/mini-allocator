#pragma once
#ifndef MM_H 
#define MM_H

#include <stdlib.h>

// alignment by 2 words (8 bytes)

#define WSIZE 4  /* Word and header/footer size (bytes) */
#define DSIZE 8  /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12)  /* Extended heap by this amount (4K bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

// pack a size and allocated bit into a word
#define PACK(size, alloc_flag) ((size) | (alloc_flag))
// read and write a word at address ptr
#define GET(ptr) (*(unsigned int *)ptr)
#define PUT(ptr, val) (*(unsigned int *)ptr = (val))
// read the size and allocated flag bit from address ptr
// bit size depends on the first operand, note why ~0x7
#define GET_SIZE(ptr) (GET(ptr) & ~0x7)
#define GET_ALLOC(ptr) (GET(ptr) & 0x1)

//       |<- req size ->|
// +-----+--------------+---------+-----+
// | hdr |   payload    | padding | ftr |
// +-----+--------------+---------+-----+
//  WSIZE                          WSIZE
// |<-------   aligned size   --------->|
//
// given BLOCK ptr bp, compute address of its header and footer
// sizeof(header) = sizeof(footer) = word size
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
// given block ptr bp, compute address of next and previous blocks (block addr)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
// get previous aligned size from footer of the previous block
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

int mm_init();
void *mm_malloc(size_t payload_size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t payload_size);

static void *extend_heap(size_t block_words);
static void *coalesce(char *bp);
static void *find_fit(size_t size_aligned);
static void place(char *bp, size_t size_aligned);

#endif  // MM_H
