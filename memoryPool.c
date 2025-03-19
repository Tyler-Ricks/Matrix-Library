#include "memoryPool.h"

void printVoidPtr(void* ptr) {
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

pool createPool(int size) {
	void* start;
	if ((start = malloc(size)) == NULL) {
		printf("palloc allocation failed, returning pool of NULL\n");
		return (pool){NULL, NULL, NULL};
	}
	// uintptr_t guarantees start to be the size of a pointer, which allows 
	// it to be added to integer size to locate the end address.
	void* end = (void*)((uintptr_t)start + size);
	return (pool) {
		start,
		end,
		end // set ptr to end for downward bumping
	};
}

// Because the pool bumps downward, realloc will take O(n) time. (start must be at a lower index, which
// and typical realloc really only works upwards. The solution is to allocate a whole different, resized
// chunk of memory, then copy the original pool to the new one. Please just avoid realloc please
void* poolrealloc(pool* frame, int input_size) {
	int old_size = ((uintptr_t)frame->end - (uintptr_t)frame->start);

	if (old_size + input_size > POOL_SIZE_CAP) {
		printf("Pool size cap hit, returning NULL");
		return NULL;
	}
	int new_size = old_size * GROWTH_FACTOR;
	if (new_size > POOL_SIZE_CAP) { new_size = POOL_SIZE_CAP; }
	if (new_size < old_size + input_size) { 
		new_size = GROWTH_FACTOR * (old_size + input_size);
	}

	//return realloc(frame->start, size);
	void* start;
	if ((start = malloc(new_size)) == NULL) {
		printf("realloc failed, returning original pool\n");
		return frame->start;
	}

	// documentation for how I found memcpy pamas
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
	int allocated = (uintptr_t) frame->end - (uintptr_t) frame->ptr;
	memcpy((uintptr_t) start + new_size - allocated, frame->ptr, allocated);

	free(frame->start);

	frame->start = start;
	frame->end = (void*)((uintptr_t)start + new_size);;
	frame->ptr = frame->end;

	// returns a pointer to the start, for usage in palloc
	return frame->start;
}

// allocates memory from pool, and bumps ptr down by size
// if bump exceeds frame size, do nothing for now, but configure it
// to realloc later
void* palloc(pool* frame, void* input, int size) {

	void* bump = (void*)((uintptr_t) frame->ptr - size);

	// This is just a safety feature. Please preallocate enough memory for the pool and don't rely on this
	if ((uintptr_t)bump < (uintptr_t)frame->start) { // bump steps past the start address of our frame
		printf("palloc ran out of space in frame, reallocating\n");
		//return(NULL);
		poolrealloc(frame, size);
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

void poolFree(pool* frame) {
	free(frame->start);
	frame->start = NULL;
	frame->end = NULL;
	frame->ptr = NULL;
}

int main() {
	pool intPool = createPool(sizeof(int) * 2);
	int x = 4;
	int* a = (int*)palloc(&intPool, &x, sizeof(int));
	int y = 128;
	int* b = (int*)palloc(&intPool, &y, sizeof(int));

	printf("x: %d, y: %d\n", *a, *b);

	int z = 500;
	int* c = (int*)palloc(&intPool, &z, sizeof(int)); // should fail, because it runs out of space

	printf("z: %d", *c);

	poolFree(&intPool);
}