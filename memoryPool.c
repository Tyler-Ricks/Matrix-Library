// Refactor checklist:
//        1) rewrite pool_alloc
//		  2) rewrite raw_pool_alloc
//		  3) rewrite pool_realloc
//		  4) rewire pool_free
//		  5) maybe have pool store size instead of an end pointer? it's unnecessary at this point I think but still
//


// rework memoryPool! This may just be a complete redo of the whole thing, depending on where my thoughts go 
// I would like realloc to work. I don't want to use it overall, but I want my system to have it be available 
// 
// The Problem:
// Things using memoryPool (fmatrix) rely on maintaining pointers to the data stored on the pool. 
// pool_realloc resizes the pool by using realloc()
// however, realloc() happens to move all the allocated data to a new location
// So, I either need a way to update pointers maintained by an fmatrix, or change how memoryPool works entirely.
// 
// One possibility for changing how memoryPool works is to avoid realloc(). 
// - This could be done with linked lists. The pool struct could store a pointer to the next pool, and we could
//   make it so pool_realloc simply creates a new pool, and lets the old pool point to the new one. This would
//   make allocation a bit tricky, as I'd need to decide if it's possible to store memory across two pools as
//   to not waste space, or to waste space by just allocating the whole chunk on the new pool. For the latter
//   option, maybe it's possible to free up the unneeded space?
// - maybe have a similar idea to the linked list, but instead have a max priority queue of allocated pools
//   sorted by reaining space?
//

#include "memoryPool.h"

// prints a void pointer
// used mostly for debugging if you run into memory issues
//		
// printf("A.start:\n");
// print_void_ptr(A.start);
// printf("\nA.end:\n);
// print_void_ptr(A.end);
void print_void_ptr(void* ptr) {
	printf("%p", ptr);
}
// POOL STRUCT REF
/*
typedef struct {
void* start;
void* end;
// start is at a low address and end is at a high one.
// thus: start <= ptr <= end
// If we try to allocate beyond this, either disallow it, or realloc
void* ptr;
}pool;
*/

// Chceks if place is inside frame's bounds, returns false if not
// Used for pool_free_from, but may refactor pool_alloc with it to make it cleaner
int is_in_pool(pool frame, void* place) {
	return ((char*)frame.start <= (char*)place) && ((char*)frame.end >= (char*)place);
}

// Allocates size bytes, and returns a pool struct of pointers to memory's start, end, and current levels
//   - ptr indicates the current level, and is initialized to the first space in memory
//   - if it fails, it returns a pool with all members set to NULL
// Use to dynamically allocate stuff without always using malloc/calloc
// 
// pool frame = create_pool((rows * columns * 5) * sizeof(float));
pool create_pool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("createPool allocation failed, returning pool of NULL\n");
		return (pool){NULL, NULL, NULL};
	}
	void* end = (char*)start + size;
	return (pool) {
		start,
		end,
		start, // set ptr to start for upward bumping
		NULL
	};
}

new_pool new_create_pool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("createPool allocation failed, returning pool of NULL\n");
		return (new_pool){NULL, 0, NULL, NULL};
	}

	return (new_pool) {
		start,				// start of pool
		size,				// size of pool
		start,				// first available spot in pool
		NULL				// pointer to next pool
	};
}

// allocates a pool on the heap
// used by pool_realloc to create a new pool that doesn't just exist on the stack
new_pool* heap_create_pool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("failed to allocate memory for a new pool, returning NULL\n");
		return NULL;
	}

	new_pool* result;
	if ((result = malloc(sizeof(new_pool))) == NULL) {
		printf("failed to allocate memory for a pool struct, returning NULL\n");
		return NULL;
	}

	result->start = start;
	result->size = size;
	result->ptr = start;
	result->next = NULL;
}

// rewriting pool_alloc/realloc:
// It seems like an almost full rewrite is necessary. If I am rolling with the linked list of pools implementation,
// I need to be able to know which pool to allocate on, and update that after "reallocating"

// moves the end pointer upwards, allowing for more memory to be allocated on the pool. Returns unmoved ptr otherwise
//   - returns NULL if reallocation fails, or POOL_CAP_SIZE is exceeded
//   - increases pool size by GROWTH_FACTOR * previous size
//       > if this is not big enough for the new input, then it increases it by GROWTH_FACTOR * (old_size + input_size)
//
void* pool_realloc(pool* frame, int input_size) {

	int old_size = ((char*)frame->end - (char*)frame->start);
	int offset = (char*)frame->ptr - (char*)frame->start;		// for updating where the ptr goes

	if (old_size + input_size > POOL_SIZE_CAP) {
		printf("Pool size cap hit, returning NULL\n");
		return NULL;
	}
	int new_size = old_size * GROWTH_FACTOR;
	if (new_size < old_size + input_size) { // checks for new size not being enough for the input
		new_size = GROWTH_FACTOR * (old_size + input_size);
	}
	if (new_size > POOL_SIZE_CAP) { new_size = POOL_SIZE_CAP; } // checks for new size being larger than cap

	// note: the last two if statements are not enough to cover the case where input size would cause the pool cap to
	// be exceeded, but that possibility is handled earlier in the function
	void* check;
	if ((check = realloc(frame->start, new_size)) == NULL) {
		printf("realloc failed, returning NULL\n");
		return NULL;
	}
	frame->start = check; // so if you free something that's been realloc'd, you have to pass in the pointer that was
						  // returned by the realloc. They would be equivalent in most cases, but I think if realloc
						  // had to move the memory block elsewhere, this would matter (but it would give me errors 
						  // for accessing stuff pointing to locations in the pool anyways) 

	frame->ptr = (char*)frame->start + offset;
	frame->end = (char*)frame->start + new_size;
}

// checks all allocated pools for room for the input. 
// If one is found, return a pointer to that pool
// If not, allocate a new pool with a new size, then return a pointer to it

void* new_pool_realloc(new_pool* frame, int input_size) {

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
	if (new_size < frame->size + input_size) { // checks for new size not being enough for the input
		new_size = GROWTH_FACTOR * (frame->size + input_size);
	}
	if (new_size > POOL_SIZE_CAP) { new_size = POOL_SIZE_CAP; } // checks for new size being larger than cap

	new_pool* newPool = heap_create_pool(new_size);
	if (newPool == NULL) {
		print("pool reallocation failed!\n");
		exit(1);
	}

	// set the previous pool's next pointer to the new pool
	frame->ptr = newPool;

	return newPool;
}

// Allocates memory from the pool, copies input into that memory. Returns a pointer to the start of that memory
//   - if it fails, it returns NULL
// Used for allocating space for already declared things like matrices
// 
// float in = 2.5;
// float* x = pool_alloc(&frame, &in, sizeof(float)); // x now points to 2.5
void* pool_alloc(pool* frame, void* input, int size) {

	void* bump = (char*) frame->ptr + size;
	void* check = frame->end;

	// realloc is just a safety feature. Please preallocate enough memory for the pool and don't rely on this
	if ((char*)bump > (char*)frame->end) { // checks if bump steps out of the pool
		printf("palloc ran out of space in frame, reallocating\n");
		//return(NULL); // realloc is banned until further notice
		check = pool_realloc(frame, size); // after realloc, frame->end will be at a higher index
		//bump = (char*) frame->ptr - size;
	}

	if (!check) { // catches failed realloc
		printf("pool_realloc failed, returning NULL\n");
		return NULL;
	}

	// store an input 
	// ptr indicates the next free byte, so copy to ptr from input
	void* result = memcpy(frame->ptr, input, size);

	// bump ptr to new value
	frame->ptr = bump;

	// Return pointer to the start of what we just allocated
	//printf("%g, %g\n", *((float*)input), *((float*)result));
	return result;
}

// tests if pool frame is large enough for an input of input_size
// returns true if 
int pool_has_capacity(new_pool* frame, int input_size) {
	int new_size = ((char*)frame->ptr + input_size) - (char*)frame->start;
	return(new_size <= frame->size);
}

// allocates memory on an input pool, then copies input data into that memory
// returns a pointer to the allocated memory
void* new_pool_alloc(new_pool* frame, void* input, int input_size) {
	if (!pool_has_capacity) {
		printf("must reallocate, but we don't do that yet\n");
		exit(0);
	}

	void* result = memcpy(frame->ptr, input, input_size);	// copy data from input
	frame->ptr = (char*)frame->ptr + input_size;			// update the start ptr in frame
	return result;											// return pointer to the start of allocated data
}

// Allocates memory from the pool. Returns a pointer to the start of that memory
//   - if it fails, it returns NULL
// Used for allocating space to fill later
// 
// float* x = raw_pool_alloc(&frame, sizeof(float));
// *x = 2.5;
void* raw_pool_alloc(pool* frame, int size) {

	void* bump = (char*) frame->ptr + size;
	void* check = frame->end;

	// This is just a safety feature. Please preallocate enough memory for the pool and don't rely on this
	if ((char*)bump > (char*)frame->end) { // checks if bump steps out of the pool
		printf("palloc ran out of space in frame, reallocating\n");
		//return(NULL); // realloc is banned for now
		check = pool_realloc(frame, size);
		bump = (char*) frame->ptr + size;
	}

	if (!check) { // catches failed realloc
		printf("pool_realloc failed, returning NULL\n");
		return NULL;
	}

	// bump ptr to new value
	void* result = frame->ptr;
	frame->ptr = bump;

	// Return pointer to the start of what we just allocated
	// We bump allocate downwards, so the address to the allocated memory is just bump 
	return result;
}

// allocates a block of an input size onto the input pool
// returns a pointer to the allocated memory
void* new_raw_pool_alloc(new_pool* frame, int size) {
	if (!pool_has_capacity) {
		printf("must reallocate, but we don't do that yet\n");
		exit(0);
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


// frees all memory in a pool
// used after you don't need stuff from the pool anymore, then sets pool pointers to NULL
// 
// free_pool(&frame);
void free_pool(pool *frame) {
	free(frame->start);
	frame->start = NULL;
	frame->end = NULL;
	frame->ptr = NULL;
	frame->next = NULL;
}


void new_free_pool(new_pool* frame) {
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
void heap_free_pool(new_pool* frame) {
	if (frame->next != NULL) {
		new_free_pool(frame->next);
	}
	free(frame->start);
	free(frame);
}