#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy

#define POOL_SIZE_CAP 16000
#define GROWTH_FACTOR 1.5

void print_void_ptr(void* ptr);

// memory pool stuff (simple bump allocator)

// struct storing various pointers to our allocated pool
// creating a pool should malloc out the memory, then return a pool struct
// with all these pool pointers
typedef struct {
	void* start;
	void* end;
	// start is at a low address and end is at a high one.
	// thus: start <= ptr <= end
	// If we try to allocate beyond this, either disallow it, or realloc
	void* ptr;
}pool;

pool create_pool(int size);

void* pool_realloc(pool* frame, int input_size);

void* pool_alloc(pool* frame, void* input, int size);
void* raw_pool_alloc(pool* frame, int size);

void free_pool(pool*);

#endif
