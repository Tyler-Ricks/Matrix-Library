#ifndef MATRIX_H
#define MATRIX_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "memoryPool.h"

// macro so I can just use a 1D array for matrix struct
#define MATRIX_AT(mat, i, j) (mat.matrix[i, i * mat.n + j])

// float matrix
typedef struct{
	// rows, columns
	int m, n;
	// pointer to item at [0,0]
	float* matrix; 
}fmatrix;

fmatrix create_fmatrix(int m, int n, float* matrix, pool* frame);

fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame);

void print_matrix(fmatrix mat);

#endif MATRIX_H
