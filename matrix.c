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

fmatrix fmatrix_scale(fmatrix mat, float c, pool *frame) {
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, mat.m * mat.n * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	for (int i = 0; i < mat.m; i++) {
		for (int j = 0; j < mat.n; j++) {
			result[INDEX_AT(mat, i, j)] = c * MATRIX_AT(mat, i, j);
		}
	}

	return (fmatrix){ mat.m, mat.n, result};
}

// does the dot product of row i of matA by column j of matB
float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j) {
	float result = 0;

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


// Elementary row operations

// Row should be 0-indexed
fmatrix fmatrix_row_scale(fmatrix mat, int row, float c, pool *frame) {
	if (row >= mat.m) {
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

fmatrix fmatrix_row_swap(fmatrix mat, int row1, int row2, pool *frame) {
	if (row1 >= mat.m) {
		printf("row_swap error: \nrow1 %d out of bounds (make sure you are 0-indexed)\n", row1);
		return (fmatrix){ 0, 0, NULL};
	}
	if (row2 >= mat.m) {
		printf("row_swap error: \nrow2 %d out of bounds (make sure you are 0-indexed)\n", row2);
		return (fmatrix){ 0, 0, NULL};
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	for (int i = 0; i < mat.n; i++) {
		fswap(&result.matrix[INDEX_AT(mat, row1, i)], 
			  &result.matrix[INDEX_AT(mat, row2, i)]);
	}

	return result;
}

fmatrix fmatrix_row_sum(fmatrix mat, int dest, float c1, int src, float c2, pool* frame) {
	if (src >= mat.m) {
		printf("row_sum error: \src row %d out of bounds (make sure you are 0-indexed)\n", src);
		return (fmatrix){ 0, 0, NULL};
	}
	if (dest >= mat.m) {
		printf("row_sum error: \dest row %d out of bounds (make sure you are 0-indexed)\n", dest);
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

	float matrixb[3][1] = {{1.0}, 
						   {2.0},
						   {3.0}};
	fmatrix B = create_fmatrix(3, 1, matrixb, &frame);
	
	printf("A:\n");
	print_fmatrix(A);

	printf("\nA[0][0]R2 - A[1][0]R1\n");
	fmatrix result = create_fmatrix(rows, columns, matrixa, &frame);
	result = fmatrix_row_sum(result, 1, MATRIX_AT(result, 0, 0), 0, -MATRIX_AT(result, 1, 0), &frame);

	print_fmatrix(result);

	printf("\n R2 * 2\n");
	print_fmatrix(fmatrix_row_scale(A, 1, 2.0, &frame));

	// free the memory pool after its use
	free_pool(&frame);
}