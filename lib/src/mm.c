#include "mm.h"
#include "memlib.h"

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

  // extend the empty heap with a free block of CHUNKSIZE bytes
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
    return -1;
  }

  return 0;
}

/*! TODO: mm_malloc and mm_realloc
 *
 * @todo Cure my dementia.
 */
void *mm_malloc(size_t size) {
  if (size == 0) return NULL;

  size_t size_aligned;
  // adjust block size to include overhead (header and footer) and alignment reqs
  // alignment by DSIZE
  if (size <= DSIZE) {
    // header and footer and a payload with DSIZE
    size_aligned = 2 * DSIZE;
  } else {
    // size (payload) + DSIZE (hdr/ftr)
    // (DSIZE - 1) / DSIZE (round up)
    size_aligned = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
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
  return bp;
}

void *mm_realloc(void *ptr, size_t size);

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

static void *extend_heap(size_t words) {
  // allocate even number of words to maintain alignment
  size_t size = words % 2 ? (words + 1) * WSIZE : words * WSIZE;

  char *bp = NULL;
  if ((long)(bp = mem_sbrk(size)) == -1) {
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

static void *find_fit(size_t size_aligned) {

}

static void *place(char *bp, size_t size_aligned) {

}
