#include "mm.h"
#include "memlib.h"
#include <string.h>

static char *heap_listp;

/*!
 * @brief mm_init create the initial empty heap with following structure
 *
 * +-----+-----+-----+-----+
 * | pad | hdr | ftr | hdr |
 * +-----+-----+-----+-----+
 *       | prologue  | epilogue |
 *
 * return 0 if succuess, -1 otherwise
 */
int mm_init() {
  // create the initial empty heap
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1l) {
    return -1;
  }

  PUT(heap_listp, 0); /* alignment padding for the first word */
  heap_listp += WSIZE;
  PUT(heap_listp, PACK(DSIZE, 1)); /* prologue header */
  heap_listp += WSIZE;
  PUT(heap_listp, PACK(DSIZE, 1)); /* prologue footer */
  // epilogue has the size of a special value 0
  heap_listp += WSIZE;
  PUT(heap_listp, PACK(0, 1)); /* epilogue header */

  // heap_listp points to the payload(0) of prologue block
  heap_listp -= WSIZE;

  // extend the empty heap with a free block with size of CHUNKSIZE bytes
  // NOTE: block size != payload size
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
    return -1;
  }

  return 0;
}

/*!
 * @brief: mm_malloc aligns up payload size and find a fit for that block.
 *
 * return the pb for the block
 */
void *mm_malloc(size_t payload_size) {
  if (payload_size == 0)
    return NULL;

  size_t size_aligned;
  // adjust block payload_size to include overhead (header and footer) and
  // alignment  // reqs alignment by DSIZE
  if (payload_size <= DSIZE) {
    // header and footer and a payload with DSIZE
    size_aligned = 2 * DSIZE;
  } else {
    // payload_size (payload) + DSIZE (hdr/ftr)
    // (DSIZE - 1) / DSIZE (round up)
    size_aligned = DSIZE * ((payload_size + (DSIZE) + (DSIZE - 1)) / DSIZE);
  }

  char *bp = NULL;
  // search the free list for a fit
  if ((bp = find_fit(size_aligned)) != NULL) {
    place(bp, size_aligned);
    return bp;
  }

  // no fit found
  size_t extendsize = MAX(size_aligned, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL) {
    return NULL;
  }
  place(bp, size_aligned);
  return (void *)bp;
}

/*!
 * @brief mm_realloc aligns payload size up. There are two cases:
 *
 * - meet split condition
 *   split memory in-place
 *
 *    |<-----------  old size  ---------->|
 *    +-----+-----+-----+-----+-----+-----+
 *    | hdr | pay | ftr | hdr | pay | ftr |
 *    +-----+-----+-----+-----+-----+-----+
 *         allocated           free
 *    |<-- new size  -->|  DSIZE at least
 *
 * - do not meet split condition
 *   find a fit, and copy content in original block to new block as much as 
 *   possible, and finally free original block.
 *
 * return new bp if succuess, old bp otherwise
 */
void *mm_realloc(void *bp, size_t payload_size) {
  // adjust payload_size to alignment reqs
  size_t new_size;
  if (payload_size <= DSIZE) {
    new_size = 2 * DSIZE;
  } else {
    new_size = DSIZE * ((payload_size + (DSIZE) + (DSIZE - 1)) / DSIZE);
  }

  char *new_bp = NULL;
  size_t old_size = GET_SIZE(HDRP(bp));
  if (old_size >= new_size + DSIZE) {
    // split in-place
    size_t free_size = old_size - new_size;
    PUT(FTRP(bp), PACK(free_size, 0));
    PUT(HDRP(bp), PACK(new_size, 1));
    PUT(FTRP(bp), PACK(new_size, 1));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(free_size, 0));

    coalesce(NEXT_BLKP(bp));
    new_bp = bp;
  } else {
    // allocate a new space
    if ((new_bp = (char *)mm_malloc(payload_size)) == NULL) {
      return bp;
    }
    // copy min(payload_size of new block, payload of old block)
    memcpy(new_bp, bp, MIN(payload_size, old_size - DSIZE));
    mm_free(bp);
  }
  return new_bp;
}

/*!
 * @brief mm_free free current block and coalesce if necessary
 *
 * return nothing
 */
void mm_free(void *bp) {
  size_t size = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

/*!
 * @brief extend_heap extends heap by moving brk by calling mem_sbrk routine
 *
 * @words: block size, NOT payload size
 *
 * return block pointer which points to the start address of payload
 */
static void *extend_heap(size_t words) {
  // allocate even number of words to maintain DSIZE alignment
  size_t size = words % 2 ? (words + 1) * WSIZE : words * WSIZE;

  char *bp = NULL;
  if ((long)(bp = mem_sbrk(size)) == -1l) {
    return NULL;
  }

  // initialize free block header/footer and the epilogue header for new
  // extended storage. Note the old epilogue is occupied by the header of new
  // block. That is the function of epilogue, which consists of only single-word
  // header.
  PUT(HDRP(bp), PACK(size, 0));         /* free block header */
  PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

  // coalesce if the previous block was free
  return coalesce(bp);
}

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

/*!
 * @brief find_fit find a fit storage for the request
 * 
 * we will implement the first-fit algorithm for find_fit function
 *
 * return a pointer for available space
 */
static void *find_fit(size_t size_aligned) {
  for (void *bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (!GET_ALLOC(HDRP(bp)) && (size_aligned < GET_SIZE(HDRP(bp)))) {
      return bp;
    }
  }
  return NULL;
}

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
