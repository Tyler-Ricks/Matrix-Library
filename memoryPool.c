// Refactor checklist:

#include "memoryPool.h"

// prints a void pointer
// used mostly for debugging if you run into memory issues
//		
// printf("A.start:\n");
// print_void_ptr(A.start);
void print_void_ptr(void* ptr) {
	printf("%p", ptr);
}

// Checks if place is inside frame's bounds, returns false if not
// Used for pool_free_from, but may refactor pool_alloc with it to make it cleaner
int is_in_pool(pool frame, void* place) {
	return ((char*)frame.start <= (char*)place) && ((char*)frame.start + frame.size >= (char*)place);
}

// Allocates size bytes, and returns a pool struct
//   - ptr indicates the next free memory spot, and is initialized to the first space in memory
//   - if it fails, it returns a pool with all members set to NULL
// Use to dynamically allocate stuff without always using malloc/calloc
// 
// pool frame = create_pool((rows * columns * 5) * sizeof(float));
pool create_pool(int size) {
	void* start = malloc(size);
	if (start == NULL) {
		printf("createPool allocation failed, returning pool of NULL\n");
		return (pool){NULL, -1, NULL, NULL};
	}

	return (pool) {
		start,			// start of pool
		size,			// size of pool
		start,			// first available spot in pool
		NULL			// pointer to next pool
	};
}

// allocates a pool on the heap
// used by pool_realloc to create a new pool that doesn't just exist on the stack,
// so it can be referenced later by a previous pool
// returns NULL on malloc failure
pool* heap_create_pool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("failed to allocate memory for a new pool, returning NULL\n");
		return NULL;
	}

	pool* result = malloc(sizeof(pool));
	if (result == NULL) {
		printf("failed to allocate memory for a pool struct, returning NULL\n");
		return NULL;
	}

	result->start = start;
	result->size = size;
	result->ptr = start;
	result->next = NULL;

	return result;
}

// tests if pool frame is large enough for an input of input_size
// returns true if 
int pool_has_capacity(pool* frame, int input_size) {
	int new_size = ((char*)frame->ptr + input_size) - (char*)frame->start;
	return(new_size <= frame->size);
}

// checks all allocated pools for room for the input. 
// If one is found, return a pointer to that pool
// If not, allocate a new pool with a new size, then return a pointer to it
// may be worth forcing multiples of 4 for pool size? it shouldn't be too hard to facilitate
//   - returns NULL if reallocation fails, or POOL_CAP_SIZE is exceeded
//   - increases pool size by GROWTH_FACTOR * previous size
//       > if this is not big enough for the new input, then it increases it by GROWTH_FACTOR * (old_size + input_size)
pool* pool_realloc(pool* frame, int input_size) {

	// locate the first pool with capacity
	while (frame->next != NULL) {
		frame = frame->next;
		if (pool_has_capacity(frame, input_size)) { // if a pool with capacity is found, return it
			return frame; 
		}
	}
	
	// determine new pool size
	if (frame->size + input_size > POOL_SIZE_CAP) {
		printf("Pool size cap hit, returning NULL\n");
		return NULL;
	}
	int new_size = frame->size * GROWTH_FACTOR;
	if(new_size == frame->size){ new_size = 2 * frame->size; }		// in case initial size is 1
	if (new_size < input_size) {									// checks for new size not being enough for the input
		new_size = GROWTH_FACTOR * (frame->size + input_size);
	}
	if (new_size > POOL_SIZE_CAP) { new_size = POOL_SIZE_CAP; }		// checks for new size being larger than cap

	pool* new_pool = heap_create_pool(new_size);
	if (new_pool == NULL) {
		printf("pool reallocation failed!\n");
		exit(1);
	}

	// set the previous pool's next pointer to the new pool
	frame->next = new_pool;

	return new_pool;
}

// Allocates memory from the pool, copies input into that memory. Returns a pointer to the start of that memory
//   - if it fails, it returns NULL
// Used for allocating space for already declared things like matrices
// 
// float in = 2.5;
// float* x = pool_alloc(&frame, &in, sizeof(float)); // x now points to 2.5
void* pool_alloc(pool* frame, void* input, int input_size) {
	if (!pool_has_capacity(frame, input_size)) {
		printf("pool ran out of space, reallocating\n");
		frame = pool_realloc(frame, input_size);
	}

	if (frame == NULL) {
		printf("cannot try to allocate on a NULL frame\n");
		exit(1);
	}

	void* result = memcpy(frame->ptr, input, input_size);	// copy data from input
	frame->ptr = (char*)frame->ptr + input_size;			// update the start ptr in frame
	return result;											// return pointer to the start of allocated data
}

// Allocates memory from the pool, but doesn't give it a value.
// Returns a pointer to the start of that memory
//   - if it fails, it returns NULL
// Used for allocating space to fill later
// 
// float* x = raw_pool_alloc(&frame, sizeof(float));
// *x = 2.5;
void* raw_pool_alloc(pool* frame, int size) {
	if (!pool_has_capacity(frame, size)) {
		printf("pool ran out of space, reallocating\n");
		frame = pool_realloc(frame, size);
	}

	void* result = frame->ptr;
	frame->ptr = (char*)frame->ptr + size;
	return result;
}


// frees memory in frame after pointer start
// Literally just moves frame->ptr to *start
// returns NULL if start is not in frame's range
// Used for some operations that want to operate on a duplicate of an input, but not store that duplicate 
// for later (ex determinant by triangulation wants to not modify the input matrix, but have a matrix to 
// do row operations on, so it duplicates input, then gets its determinant, then frees it with this
void* pool_free_from(pool* frame, void* start) {
	return is_in_pool(*frame, start) ? frame->ptr = start : printf("pool_free_from failed: input pointer not in frame!\n"); return NULL;
}

void free_pool(pool* frame) {
	if (frame->next != NULL) {
		heap_free_pool(frame->next);
	}
	free(frame->start);
	frame->start = NULL;
	//frame->size = 0;
	frame->size -1; //maybe set to negative for checks
	frame->ptr = NULL;
	frame->next = NULL;
}

// recursively frees pools from the end of the chain to the start
// the first pool is not on the heap. Don't free it
void heap_free_pool(pool* frame) {
	if (frame->next != NULL) {
		free_pool(frame->next);
	}
	//printf("freeing pool with size %d\n", frame->size);
	free(frame->start);
	free(frame);
}