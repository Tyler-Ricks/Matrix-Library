#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy

#define POOL_SIZE_CAP 16000
#define GROWTH_FACTOR 1.5

void print_void_ptr(void* ptr);


// memory pool stuff (simple bump allocator)

typedef struct {
	void* start;				// pointer to start of allocated memory
	int size;					// max size of the pool in number of bytes
	void* ptr;					// pointer to the first free spot in the pool
	struct pool* next;			// pointer to the next allocated pool
}pool;

int is_in_pool(pool frame, void* place);

pool create_pool(int size);
pool* heap_create_pool(int size);

int pool_has_capacity(pool* frame, int input_size);

pool* pool_realloc(pool* frame, int input_size);

void* pool_alloc(pool* frame, void* input, int input_size);
void* raw_pool_alloc(pool* frame, int size);

void* pool_free_from(pool* frame, void* start);
void free_pool(pool* frame);
void heap_free_pool(pool* frame);

#endif
