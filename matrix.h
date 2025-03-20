#ifndef MATRIX_H
#define MATRIX_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "memoryPool.h"

// float matrix
typedef struct{
	// rows, columns
	int m, n;
	// pointer to item at [0,0]
	float* matrix; 
}fmatrix;

fmatrix create_fmatrix(int m, int n, float* matrix, pool* frame);

#endif MATRIX_H
