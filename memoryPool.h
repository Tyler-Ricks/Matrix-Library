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
	struct pool* next;
}pool;

/*
// potential new pool struct, stores a size instead of an end pointer
typedef struct {
	void* start;
	int size;
	// start is at a low address and end is at a high one.
	// thus: start <= ptr <= end
	// If we try to allocate beyond this, either disallow it, or realloc
	void* ptr;
	new_pool* next;
}new_pool;*/
typedef struct {
	void* start;
	int size;
	// start is at a low address and end is at a high one.
	// thus: start <= ptr <= end
	// If we try to allocate beyond this, either disallow it, or realloc
	void* ptr;
	struct new_pool* next;
}new_pool;

int is_in_pool(pool frame, void* place);

pool create_pool(int size);

void* pool_realloc(pool* frame, int input_size);

void* pool_alloc(pool* frame, void* input, int size);
void* raw_pool_alloc(pool* frame, int size);

new_pool new_create_pool(int size);
new_pool* heap_create_pool(int size);

int pool_has_capacity(new_pool* frame, int input_size);

void* new_pool_realloc(new_pool* frame, int input_size);

void* new_pool_alloc(new_pool* frame, void* input, int input_size);
void* new_raw_pool_alloc(new_pool* frame, int size);

void* pool_free_from(pool* frame, void* start);
void free_pool(pool *frame);

void new_free_pool(new_pool* frame);
void heap_free_pool(new_pool* frame);

#endif
