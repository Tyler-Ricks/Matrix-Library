#include "matrix.h"

// allocates m by n blocks of memory of a given size in a pool
fmatrix create_fmatrix(int m, int n, float* matrix, pool* frame) {
	if (!frame || !frame->start) {
		printf("failed to create matrix (faulty input frame). Returning empty matrix\n");
		return (fmatrix) {0, 0, NULL};
	}

	if ((matrix = (float*)pool_alloc(frame, matrix, m * n * sizeof(float))) == NULL) {
		printf("pool allocation for matrix failed, returing empty matrix\n");
		return (fmatrix) {0, 0, NULL};
	}

	return (fmatrix) {m, n, matrix};
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
		return (fmatrix) {0, 0, NULL};
	}

	int i, size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while adding: pool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	for (i = 0; i < size; i++) {
		result[i] = matA.matrix[i] + matB.matrix[i];
	}

	return (fmatrix) {matA.m, matA.n, result};
}

fmatrix fmatrix_subtract(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while subtracting: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return (fmatrix) {0, 0, NULL};
	}

	int i, size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	for (i = 0; i < size; i++) {
		result[i] = matA.matrix[i] - matB.matrix[i];
	}

	return (fmatrix) {matA.m, matA.n, result};
}

fmatrix fmatrix_scale(fmatrix mat, float c, pool* frame) {
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, mat.m * mat.n * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	int i, j;
	for (i = 0; i < mat.m; i++) {
		for (j = 0; j < mat.n; j++) {
			result[INDEX_AT(mat, i, j)] = c * MATRIX_AT(mat, i, j);
		}
	}

	return (fmatrix){ mat.m, mat.n, result};
}

// does the dot product of row i of matA by column j of matB
float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j) {
	float result = 0;
	int a;
	for (a = 0; a < matA.n; a++) {
		result += (MATRIX_AT(matA, i, a)) * (MATRIX_AT(matB, a, j));
	}

	return result;
}

// basic brute force matrix multiplication 
fmatrix fmatrix_multiply(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.n != matB.m) {
		printf("error while multiplying: \ndimension mismatch: ");
		printf("matrix a: (%d x _%d_)  matrix b: (_%d_ x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return (fmatrix) {0, 0, NULL};
	}
	// new matrix has row count of A and col count of B
	int size = matA.m * matB.n;
	float* result;
	if ((result = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while subtracting: \npool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	int i, j;
	for (i = 0; i < matA.m; i++) {
		for(j = 0; j < matB.n; j++){
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
		return (fmatrix) {0, 0, NULL};
	}

	int i, j;
	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++){
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
		return (fmatrix){ 0, 0, NULL};
	}

	float* result;
	int size = mat.m * mat.n * sizeof(float);
	if ((result = (float*)raw_pool_alloc(frame, size)) == NULL) {
		printf("error while row scaling: \npool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	result = memcpy(result, mat.matrix, size);

	int i;
	for (i = 0; i < mat.n; i++) {
		result[INDEX_AT(mat, row, i)] *= c;
	}

	return (fmatrix) {mat.m, mat.n, result};
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

	float* result;
	int size = mat.m * mat.n * sizeof(float);
	if ((result = (float*)raw_pool_alloc(frame, size)) == NULL) {
		printf("error while row scaling: \npool allocation failure\n");
		return (fmatrix) {0, 0, NULL};
	}

	result = memcpy(result, mat.matrix, size);

	int i;
	for (i = 0; i < mat.n; i++) {
		fswap(&result[INDEX_AT(mat, row1, i)], &result[INDEX_AT(mat, row2, i)]);
	}

	return (fmatrix) {mat.m, mat.n, result};
}


// Utilities

void print_matrix(fmatrix mat) {
	// null matrix has 0 dimension, so doesn't print
	int i, j;
	for (i = 0; i < mat.m; i++) {
		for (j = 0; j < mat.n; j++) {
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
	print_matrix(A);
	printf("\nB:\n");
	print_matrix(B);
	printf("\n");

	printf("A x B:\n");
	print_matrix(fmatrix_multiply(A, B, &frame));

	printf("\ntranspose(A):\n");
	print_matrix(fmatrix_transpose(A, &frame));

	printf("\nA scaled by 2:\n");
	print_matrix(fmatrix_scale(A, 2.0, &frame));

	printf("\nR1 of A times 0.5:\n");
	print_matrix(fmatrix_row_scale(A, 3, 0.5, &frame));

	printf("\nR1 of A swapped with R3:\n");
	print_matrix(fmatrix_row_swap(A, 0, 2, &frame));


	// free the memory pool after its use
	free_pool(&frame);
}