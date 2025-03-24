#include "memoryPool.h"

void print_void_ptr(void* ptr) {
	printf("%p", ptr);
}

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

// prints floats from a pool for debugging
void print_pool(pool *frame) {
	for (int i = 0; i < (float*)frame->end - (float*)frame->ptr; i++) {
		printf("%g ", ((float*) frame->start)[i]);
	}
}

pool create_pool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("createPool allocation failed, returning pool of NULL\n");
		return (pool){NULL, NULL, NULL};
	}
	// uintptr_t guarantees start to be the size of a pointer, which allows 
	// it to be added to integer size to locate the end address.
	// *** I switched instances of uintptr_t to	char* ***
	void* end = (char*)start + size;
	return (pool) {
		start,
		end,
		end // set ptr to end for downward bumping
	};
}

// Because the pool bumps downward, realloc will take O(n) time. (start must be at a lower index, which
// and typical realloc really only works upwards. The solution is to allocate a whole different, resized
// chunk of memory, then copy the original pool to the new one. Please just avoid realloc please
//
// This function also modifies frame, so keep that in mind
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

	// note: the last two if statements are not enough to cover the case where input size would cause 
	// the pool cap to be exceeded, but that possibility is handled earlier in the function

	//return realloc(frame->start, size);
	void* start;
	if ((start = malloc(new_size)) == NULL) {
		printf("realloc failed, returning NULL\n");
		return NULL;
	}

	// documentation for how I found memcpy params
	/*
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
	*/

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
}

// allocates memory from pool, and bumps ptr down by size
// if bump exceeds frame size, do nothing for now, but configure it
// to realloc later
void* pool_alloc(pool* frame, void* input, int size) {

	void* bump = (char*) frame->ptr - size;
	void* check = frame->start;

	// This is just a safety feature. Please preallocate enough memory for the pool and don't rely on this
	if ((char*)bump < (char*)frame->start) { // bump steps past the start address of our frame
		printf("palloc ran out of space in frame, reallocating\n");
		//return(NULL);
		check = pool_realloc(frame, size);
	}

	if (!check) { // catches failed realloc or null frame pointer
		printf("pool_realloc failed, returning NULL\n");
		return NULL;
	}

	// store an input 
	// Because we bump downwards, the bumped ptr is the pointer to the allocated memory. So, we don't
	// need to set the result of memcpy to anything
	memcpy(bump, input, size);

	// bump ptr to new value
	frame->ptr = bump;

	// Return pointer to the start of what we just allocated
	// We bump allocate downwards, so the address to the allocated memory is just bump 
	return bump;
}

// doesn't store an input, just allocates space
void* raw_pool_alloc(pool* frame, int size) {

	void* bump = (char*) frame->ptr - size;
	void* check = frame->start;

	// This is just a safety feature. Please preallocate enough memory for the pool and don't rely on this
	if ((char*)bump < (char*)frame->start) { // bump steps past the start address of our frame
		printf("palloc ran out of space in frame, reallocating\n");
		//return(NULL);
		check = pool_realloc(frame, size);
	}

	if (!check) { // catches failed realloc or null frame pointer
		printf("pool_realloc failed, returning NULL\n");
		return NULL;
	}

	// bump ptr to new value
	frame->ptr = bump;

	// Return pointer to the start of what we just allocated
	// We bump allocate downwards, so the address to the allocated memory is just bump 
	return bump;
}

void free_pool(pool* frame) {
	free(frame->start);
	frame->start = NULL;
	frame->end = NULL;
	frame->ptr = NULL;
}

/*int main() {
	// main doesn't currently check for null returns on allocations
	pool intPool = create_pool(sizeof(int) * 2);
	int x = 4;
	int* a = (int*)pool_alloc(&intPool, &x, sizeof(int));
	int y = 128;
	int* b = (int*)pool_alloc(&intPool, &y, sizeof(int));

	printf("x: %d, y: %d\n", *a, *b);

	int z = 500;
	int* c = (int*)pool_alloc(&intPool, &z, sizeof(int)); // should fail, because it runs out of space

	printf("z: %d\n", *c);

	pool_alloc(&intPool, &a, sizeof(int)); // tries to allocate more space, but hits cap

	free_pool(&intPool);
}*/