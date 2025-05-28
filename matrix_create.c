#include "matrix.h"


// This file includes functions for creating or copying an fmatrix.
// It handles all management after a memory pool is created

// allocates m by n blocks of memory of a given size in a pool, returns a struct with a pointer to it,
// the dimensions of the matrix, and if it is a transpose or not.
// Used for adding a matrix to the pool so you can start doing operations to it.
// failure returns the ERROR_FMATRIX, (macro located in matrix.h) which has 0 rows, 0 cols, a NULL
// pointer for the matrix, and is not a transpose
//
// fmatrix A = create_fmatrix(3, 3, matA, &frame);
fmatrix create_fmatrix(int m, int n, float* matrix, pool* frame) {
	if (m < 0 || n < 0) {
		printf("fmatrix must have positive row/columns\n");
		return ERROR_FMATRIX;
	}
	if (!frame || !frame->start) {
		printf("failed to create matrix (faulty input frame). Returning empty matrix\n");
		return ERROR_FMATRIX;
	}

	if ((matrix = pool_alloc(frame, matrix, m * n * sizeof(float))) == NULL) {
		printf("pool allocation for matrix failed, returing empty matrix\n");
		return ERROR_FMATRIX;
	}
	// initially not a transpose, so field starts as 0
	return (fmatrix) {m, n, matrix, 0};
}

// returns an identity matrix of size m x n, allocated on frame
// returns ERROR_FMATRIX upon failure
fmatrix fmatrix_create_identity(int m, int n, pool* frame) {
	if (m < 0 || n < 0) {
		printf("fmatrix must have positive row/columns\n");
		return ERROR_FMATRIX;
	}
	if (!frame || !frame->start) {
		printf("failed to create matrix (faulty input frame). Returning empty matrix\n");
		return ERROR_FMATRIX;
	}

	float* matrix = raw_pool_alloc(frame, m * n * sizeof(float));
	if (matrix == NULL) {
		printf("pool allocation for identity matrix failed, returning error matrix");
		return ERROR_FMATRIX;
	}

	fmatrix mat = (fmatrix) {m, n, matrix, 0};

	// initialize all values to 0 except where i = j
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if(i == j){ matrix[INDEX_AT(mat, i, j)] = 1.0f; }
			else{ matrix[INDEX_AT(mat, i, j)] = 0.0f; }
		}
	}

	return mat;
}

// creates an m x n matrix with all elements set to x.
fmatrix fmatrix_create_full(int m, int n, float x, pool* frame) {
	if (m < 0 || n < 0) {
		printf("fmatrix must have positive row/columns\n");
		return ERROR_FMATRIX;
	}
	if (!frame || !frame->start) {
		printf("failed to create matrix (faulty input frame). Returning empty matrix\n");
		return ERROR_FMATRIX;
	}

	float* matrix = raw_pool_alloc(frame, m * n * sizeof(float));
	if (matrix == NULL) {
		printf("pool allocation for identity matrix failed, returning error matrix");
		return ERROR_FMATRIX;
	}

	fmatrix mat = (fmatrix) {m, n, matrix, 0};

	// initialize all values to 0 except where i = j
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			matrix[INDEX_AT(mat, i, j)] = 0.0f;
		}
	}

	return mat;
}

// takes an exisitng matrix, allocates space for a clone, copies its properties, and returns a deep copy
// used to reduce how verbose non inplace functions are, because many of them shared this procedure 
//
// copyA = fmatrix_copy_alloc(matA, &frame);
fmatrix fmatrix_copy_alloc(fmatrix mat, pool* frame) {
	int size = mat.m * mat.n * sizeof(float);
	float* result;

	if ((result = (float*)raw_pool_alloc(frame, size)) == NULL) {
		printf("error while allocating matrix\n");
		return ERROR_FMATRIX;
	}

	memcpy(result, mat.matrix, size);

	return (fmatrix) { mat.m, mat.n, result, mat.transpose};
}

// takes an exisitng fmatrix and a number of columns to copy, then creates a new fmatrix with 
// the first c columns of mat, allocated on frame
// for now, it does not retain mat's transpose state
fmatrix fmatrix_ncol_copy_alloc(fmatrix mat, int c, pool* frame) {
	int size = mat.m * c;
	float* result = (float*)raw_pool_alloc(frame, size * sizeof(float));

	if (result  == NULL) {
		printf("error while allocating matrix\n");
		return ERROR_FMATRIX;
	}

	int offset; // for accessing result array linearly from a nested for loop
	for (int i = 0; i < mat.m; i++) {
		offset = i * c;
		for (int j = 0; j < c; j++) {
			result[offset + j] = MATRIX_AT(mat, i, j);
			//result[offset + j] = mat.transpose ?  MATRIX_AT(mat, j, i) : MATRIX_AT(mat, i, j);
		}
	}

	return (fmatrix) { mat.m, c, result, 0};
}