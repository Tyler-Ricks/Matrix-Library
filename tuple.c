#include "tuple.h"

// I want this matrix library to be able to have functionality for different matrix decompozitions/factorizations (ex LU)
// Each of these involve an input matrix, but return multiple matrices. C doesn't have multiple returns, and I think tuples
// are a cool thing to have anyways, so I'm implementing it here. 
// I could probably just return an array of pointers to each matrix...

// Returns a tuple struct after storing pointers to each element on frame
tuple create_tuple(void* items, int n, pool* frame) {
	
}

