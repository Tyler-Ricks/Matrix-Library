#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy

#define POOL_SIZE_CAP 64000
#define GROWTH_FACTOR 1.5

void printVoidPtr(void* ptr);

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

pool createPool(int size);

void* poolRealloc(pool* frame, int input_size);

void* palloc(pool* frame, void* input, int size);

void freePool(pool*);

#endif
