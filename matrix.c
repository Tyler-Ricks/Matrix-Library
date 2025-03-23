#include "matrix.h"

// allocates m by n blocks of memory of a given size in a pool
fmatrix create_fmatrix(int m, int n, float* matrix, pool* frame) {
	if (!frame || !frame->start) {
		printf("failed to create matrix (faulty input frame). Returning empty matrix\n");
		return ERROR_FMATRIX;
	}

	if ((matrix = (float*)pool_alloc(frame, matrix, m * n * sizeof(float))) == NULL) {
		printf("pool allocation for matrix failed, returing empty matrix\n");
		return ERROR_FMATRIX;
	}

	return (fmatrix) {m, n, matrix};
}

fmatrix fmatrix_copy_alloc(fmatrix mat, pool* frame) {
	int size = mat.m * mat.n * sizeof(float);
	float* result;

	if ((result = (float*)raw_pool_alloc(frame, size)) == NULL) {
		printf("error while allocation matrix\n");
		return ERROR_FMATRIX;
	}

	memcpy(result, mat.matrix, size);

	return (fmatrix) { mat.m, mat.n, result};
}

void fswap(float *a, float *b) {
	// Aw, C doesn't allow bitwise XOR on floats to swap values
	/*
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
	* */

	float temp = *a;
	*a = *b;
	*b = temp;
}

// Basic matrix operations

fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}

	int size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while adding: pool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < size; i++) {
		result[i] = matA.matrix[i] + matB.matrix[i];
	}

	return (fmatrix) {matA.m, matA.n, result};
}

void fmatrix_add_in(fmatrix matA, fmatrix matB) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return;
	}
	int size = matA.m * matB.n;
	for(int i = 0; i < size; i++)
		matA.matrix[i] += matB.matrix[i];
}

fmatrix fmatrix_subtract(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while subtracting: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}

	int size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < size; i++) {
		result[i] = matA.matrix[i] - matB.matrix[i];
	}

	return (fmatrix) {matA.m, matA.n, result};
}

void fmatrix_subtract_in(fmatrix matA, fmatrix matB) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while subtracting: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return;
	}

	int size = matA.m * matB.n;
	for(int i = 0; i < size; i++)
		matA.matrix[i] -= matB.matrix[i];
}

fmatrix fmatrix_scale(fmatrix mat, float c, pool *frame) {
	float* result;
	int size = mat.m * mat.n;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < size; i++) {
		/*for (int j = 0; j < mat.n; j++) {
			result[INDEX_AT(mat, i, j)] = c * MATRIX_AT(mat, i, j);
		}*/
		result[i] = c * mat.matrix[i];
	}

	return (fmatrix){ mat.m, mat.n, result};
}

void fmatrix_scale_in(fmatrix mat, float c) {
	if(c == 1.0) { return; }
	int size = mat.m * mat.n;
	if(c == 0.0) { memset(mat.matrix, 0, size); return; }

	for(int i; i < size; i++)
		mat.matrix[i] *= c;
}

// does the dot product of row i of matA by column j of matB
float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j) {
	float result = 0.0;

	for (int a = 0; a < matA.n; a++) {
		result += (MATRIX_AT(matA, i, a)) * (MATRIX_AT(matB, a, j));
	}

	return result;
}

// basic brute force matrix multiplication 
fmatrix fmatrix_multiply(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.n != matB.m) {
		printf("error while multiplying: \ndimension mismatch: ");
		printf("matrix a: (%d x _%d_)  matrix b: (_%d_ x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}
	// new matrix has row count of A and col count of B
	int size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < matA.m; i++) {
		for(int j = 0; j < matB.n; j++){
			result[i * matB.n + j] = get_fmultiplied(matA, matB, i, j);
		}
	}

	return (fmatrix){ matA.m, matB.n, result};
}

// Allowing in-place matrix multiplication for non square matrices could be possible, but I would
// Have to refactor memoryPool pretty hard (reallocing space for a bigger matrix would either:
//   - overrun other memory that's been allocated
//   - have useless hanging memory if we move the new matrix elsewhere in the pool
// There are definitely ways around this, but I'll get like nothing out of it, and lose nothing
// from just doing it later.

// Also uh i didn't think enough about in place multiplication. We still need the rows and columns
// of A to be intact for straightforward matrix multiplication, but I could definitely see ways
// to get around this. I'll do it later
void fmatrix_multiply_in(fmatrix matA, fmatrix matB) {
	return; 
}

fmatrix fmatrix_transpose(fmatrix mat, pool* frame) {
	int r = mat.n, c = mat.m;

	float* result;
	if ((result = (float*)raw_pool_alloc(frame, r * c * sizeof(float))) == NULL) {
		printf("error while transposing: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++){
			result[INDEX_AT(mat, i, j)] = MATRIX_AT(mat, j, i);
		}
	}

	return (fmatrix) { r, c, result };
}

// SEE MATRIX.H COMMENT ABOUT TRANSPOSE
// gonna enforce square matrices for this too. Doing in-place trnaspose is more feasible
// than in-place multiplication because the amount of memory on the pool doesn't change,
// But the algorithm for this will take me more time to work out.

// Maybe look at this too https://www.geeksforgeeks.org/inplace-m-x-n-size-matrix-transpose/
void fmatrix_transpose_in(fmatrix mat) {
	if(mat.m != mat.n){ 
		printf("Please input a square matrix for in-place transpose\n");
		return;
	}

	for (int i = 0; i < mat.m; i++) {
		for (int j = i + 1; j < mat.n; j++) {
			//if(i == j){ continue; }
			fswap(&mat.matrix[INDEX_AT(mat, i, j)],
				  &mat.matrix[INDEX_AT(mat, j, i)]);
		}
	}
}


// Elementary row operations

// Row should be 0-indexed
fmatrix fmatrix_row_scale(fmatrix mat, int row, float c, pool *frame) {
	if (row >= mat.m || row < 0) {
		printf("row_scale error: \nrow %d out of bounds (make sure you are 0-indexed)\n", row);
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if (!result.matrix) { return result; }

	if (c == 0.0) {
		memset(result.matrix + (row * result.n), 0, result.n * sizeof(float));
		return result;
	}

	if (c == 1.0) { return result; }

	for (int i = 0; i < mat.n; i++) {
		result.matrix[INDEX_AT(result, row, i)] *= c;
	}

	return result;
}

void fmatrix_row_scale_in(fmatrix mat, int row, float c) {
	if (row >= mat.m || row < 0) {
		printf("row_scale error: \nrow %d out of bounds (make sure you are 0-indexed)\n", row);
		return;
	}
	if (c == 1.0) { return; }
	if (c == 0.0) { memset(&mat.matrix[INDEX_AT(mat, row, 0)], 0, mat.n * sizeof(float)); return; }

	for (int i = 0; i < mat.n; i++) 
		mat.matrix[INDEX_AT(mat, row, i)] *= c;
}

fmatrix fmatrix_row_swap(fmatrix mat, int row1, int row2, pool *frame) {
	if (row1 >= mat.m || row1 < 0) {
		printf("row_swap error: \nrow1 %d out of bounds (make sure you are 0-indexed)\n", row1);
		return (fmatrix){ 0, 0, NULL};
	}
	if (row2 >= mat.m || row2 < 0) {
		printf("row_swap error: \nrow2 %d out of bounds (make sure you are 0-indexed)\n", row2);
		return (fmatrix){ 0, 0, NULL};
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }
	if(row1 == row2){ return result; } // no need to change anything

	for (int i = 0; i < mat.n; i++) {
		fswap(&result.matrix[INDEX_AT(mat, row1, i)], 
			  &result.matrix[INDEX_AT(mat, row2, i)]);
	}

	return result;
}

void fmatrix_row_swap_in(fmatrix mat, int row1, int row2) {
	if (row1 >= mat.m || row1 < 0) {
		printf("row_swap error: \nrow1 %d out of bounds (make sure you are 0-indexed)\n", row1);
		return;
	}
	if (row2 >= mat.m || row2 < 0) {
		printf("row_swap error: \nrow2 %d out of bounds (make sure you are 0-indexed)\n", row2);
		return;
	}

	if(row1 == row2){ return; } // no change necessary

	for (int i = 0; i < mat.n; i++) {
		fswap(&mat.matrix[INDEX_AT(mat, row1, i)], 
			  &mat.matrix[INDEX_AT(mat, row2, i)]);
	}
}

fmatrix fmatrix_row_sum(fmatrix mat, int dest, float c1, int src, float c2, pool* frame) {
	if (dest >= mat.m || dest < 0) {
		printf("row_sum error: \dest row %d out of bounds (make sure you are 0-indexed)\n", dest);
		return (fmatrix){ 0, 0, NULL};
	}
	if (src >= mat.m || src < 0) {
		printf("row_sum error: \src row %d out of bounds (make sure you are 0-indexed)\n", src);
		return (fmatrix){ 0, 0, NULL};
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	float value; // for avoiding redundant multiplying by 0
	
	for (int i = 0; i < mat.n; i++) { 
		value = 0.0f;
		if(c1 != 0){value += (c1 * MATRIX_AT(mat, dest, i));}
		if(c2 != 0){value += (c2 * MATRIX_AT(mat, src,  i));}
		// result[INDEX_AT(mat, dest, i)] = (c1 * (MATRIX_AT(mat, dest, i))) + (c2 * (MATRIX_AT(mat, src, i)));
		result.matrix[INDEX_AT(mat, dest, i)] = value;
	}

	return result;
}

void fmatrix_row_sum_in(fmatrix mat, int dest, float c1, int src, float c2) {
	if (dest >= mat.m || dest < 0) {
		printf("row_sum error: \dest row %d out of bounds (make sure you are 0-indexed)\n", dest);
		return;
	}
	if (src >= mat.m || src < 0) {
		printf("row_sum error: \src row %d out of bounds (make sure you are 0-indexed)\n", src);
		return;
	}

	float value;

	for (int i = 0; i < mat.n; i++) {
		value = 0.0f;
		if(c1 != 0){value += (c1 * MATRIX_AT(mat, dest, i));}
		if(c2 != 0){value += (c2 * MATRIX_AT(mat, src,  i));}
		// result[INDEX_AT(mat, dest, i)] = (c1 * (MATRIX_AT(mat, dest, i))) + (c2 * (MATRIX_AT(mat, src, i)));
		mat.matrix[INDEX_AT(mat, dest, i)] = value;
	}
}

// Utilities

void print_fmatrix(fmatrix mat) {
	// null matrix has 0 dimension, so doesn't print
	for (int i = 0; i < mat.m; i++) {
		for (int j = 0; j < mat.n; j++) {
			printf("%4.3f ", MATRIX_AT(mat, i, j));
		}
		printf("\n");
	}
}

int main() {
	int rows = 3;
	int columns = 3;
	// allocate a memory pool for allocated matrices
	pool frame = create_pool((rows * columns + 3 * 1 + 100) * sizeof(float));

	float matrixa[3][3] = {{1.0, 2.0, 3.0}, 
						   {4.0, 5.0, 6.0}, 
						   {7.0, 8.0, 9.0}};
	fmatrix A = create_fmatrix(rows, columns, matrixa, &frame);
	//print_matrix(wow);

	float matrixb[3][3] = {{1.5, 2.5, 3.5}, 
						   {4.0, 5.0, 6.0}, 
						   {7.0, 8.0, 9.0}};
	fmatrix B = create_fmatrix(3, 3, matrixb, &frame);
	
	printf("A:\n");
	print_fmatrix(A);

	printf("\nB:\n");
	print_fmatrix(B);

	printf("\nA: R2 <- 3 * R2 - 2R1\n");
	fmatrix_row_sum_in(A, 1, 3.0, -3, -2.0);
	print_fmatrix(A);

	// free the memory pool after its use
	free_pool(&frame);
}