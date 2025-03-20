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

fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding matrices: dimension mismatch: ");
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
		printf("error while adding matrices: dimension mismatch: ");
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
		result[i] = matA.matrix[i] - matB.matrix[i];
	}

	return (fmatrix) {matA.m, matA.n, result};
}

void print_matrix(fmatrix mat) {
	int i, j;
	for (i = 0; i < mat.m; i++) {
		for (j = 0; j < mat.n; j++) {
			printf("%g ", MATRIX_AT(mat, i, j));
		}
		printf("\n");
	}
}

int main() {
	int rows = 3;
	int columns = 2;
	// allocate a memory pool for allocated matrices
	pool frame = create_pool(3 * rows * columns * sizeof(float));

	float matrixa[3][2] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
	fmatrix wow = create_fmatrix(rows, columns, matrixa, &frame);
	//print_matrix(wow);

	float matrixb[3][2] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
	fmatrix test = create_fmatrix(rows, columns, matrixb, &frame);
	//print_matrix(test);

	print_matrix(fmatrix_subtract(wow, test, &frame));

	// free the memory pool after its use
	free_pool(&frame);
}