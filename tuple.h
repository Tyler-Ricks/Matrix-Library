#ifndef TUPLE_H
#define TUPLE_H

#include <stdarg.h> 
#include "memoryPool.h"


// an n tuple that stores n pointers
typedef struct {
	void* items;
	int n;
}tuple;

tuple create_tuple(void* items, int n);

#endif TUPLE_H