#include "matrix.h"


// This file contains utility functions for fmatrices (mostly just printing in a bunch of ways)


// prints an input matrix in row major order, regardless of if stored in RMO or CMO
// 
// print_fmatrix(matA);
void print_fmatrix(fmatrix mat) {
	/*for (int i = 0; i < mat.m; i++) {
	for (int j = 0; j < mat.n; j++) {
	printf("%4.3f ", MATRIX_AT(mat, i, j));
	}
	printf("\n");
	}*/
	for (int i = 0; i < mat.m; i++) {
		for (int j = 0; j < mat.n; j++) {
			printf("%4.3f ", MATRIX_AT(mat, i, j));
		}
		printf("\n");
	}
}

// prints floats from a pool linearly
// used for debugging weird memory things, or tracking how transposes are stored
//
// printf("contents of frame:\n");
// print_pool(&frame);
void print_fpool(pool *frame) {
	for (int i = 0; i < (float*)frame->ptr - (float*)frame->start; i++) {
		printf("%g ", ((float*) frame->start)[i]);
	}
}

// prints the row and column count of mat, as well as if it's a transpose
//
// print_properties(matAt);
void print_properties(fmatrix mat) {
	printf("\nA.m: %d, A.n: %d, A.tranpose: %d\n", mat.m, mat.n, mat.transpose);
}

// simply prints a matrix as an array, how it is stored in memory
//
// print_as_array(matAt);
void print_as_array(fmatrix mat) {
	int count = mat.m * mat.n;
	for (int i = 0; i < count; i++) {
		printf("%g ", mat.matrix[i]);
	}
}