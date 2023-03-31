#pragma once
#ifndef MM_H 
#define MM_H

#include <stdlib.h>

int mm_init(void );
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);

#endif  // MM_H
