// Refactor checklist:
// (done) 1) refactor the pool struct to initialize ptr to start instead of end
// (done) 2) refactor pool_alloc and raw_pool_alloc
//        3) refactor pool_realloc
//		  4) change any bump downwards comments
//		  5) make better comments in general

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
		start // set ptr to start for upward bumping
	};
}

// moves the end pointer upwards, allowing for more memory to be allocated on the pool. Returns unmoved ptr otherwise
//   - returns NULL if reallocation fails, or POOL_CAP_SIZE is exceeded
//   - increases pool size by GROWTH_FACTOR * previous size
//       > if this is not big enough for the new input, then it increases it by GROWTH_FACTOR * (old_size + input_size)
//
void* pool_realloc(pool* frame, int input_size) {
	int old_size = ((char*)frame->end - (char*)frame->start);

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

	// RIP old method
	/*
	//return realloc(frame->start, size);
	void* start;
	if ((start = malloc(new_size)) == NULL) {
		printf("realloc failed, returning NULL\n");
		return NULL;
	}

	// documentation for how I found memcpy params
	// oh dear god (this is brute force implementation based on diagramming it
	// memcpy((uintptr_t) start + (new_size - old_size) + ((uintptr_t) frame->ptr - (uintptr_t) frame->start), frame->ptr, (uintptr_t) frame->end - (uintptr_t) frame->ptr);
	// First parameter is where to allocate: 
	// start of new frame +    change in size     +    space to current ptr
	//        start       + (new_size - old_size) + (frame->ptr - frame->start)
	// this can be broken down further. 
	// old_size = frame->end - frame->start, so when you sub this in, you can cancel terms:
	// start + new_size - frame->end + frame->ptr
	// 
	// The second parameter is simple enough, we are copying from frame->ptr (already allocated data)
	//
	// The third paramter is the size of what we are copying, which is simple enough too
	// (it's literally just the distance between frame->end and frame->ptr):
	// pool->end - pool->ptr
	//
	// This operation shows up in the first parameter as well, if we just factor out a negative
	// So, we store this size in variable allocated, and use it in both parameters
	// Much cleaner
	

	// better implementation does some math
	int allocated = (char*) frame->end - (char*) frame->ptr;
	// top of new chunk - already allocated stuff is an intuitive way to find where to start copying from
	memcpy((char*) start + new_size - allocated, frame->ptr, allocated);

	free(frame->start);

	frame->start = start;
	frame->end = (char*)start + new_size;
	frame->ptr = (char*)frame->end - allocated;

	// returns a pointer to the start, for usage in palloc
	return frame->start;
	*/
	void* check;
	if ((check = realloc(frame->start, new_size)) == NULL) {
		printf("realloc failed, returning NULL\n");
		return NULL;
	}
	frame->start = check; // so if you free something that's been realloc'd, you have to pass in the pointer that was
						  // returned by the realloc. They would be equivalent in most cases, but I think if realloc
						  // had to move the memory block elsewhere, this would matter (but it would give me errors 
						  // for accessing stuff pointing to locations int the pool anyways) 

	frame->end = (char*)frame->start + new_size;
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
		//bump = (char*) frame->ptr - size;
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


// frees all memory in a pool
// used after you don't need stuff from the pool anymore, then sets pool pointers to NULL
// 
// free_pool(&frame);
void free_pool(pool *frame) {
	free(frame->start);
	frame->start = NULL;
	frame->end = NULL;
	frame->ptr = NULL;
}