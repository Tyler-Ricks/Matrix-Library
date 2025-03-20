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
	pool frame = create_pool(rows * columns * sizeof(float));

	float matrix[3][2] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
	fmatrix wow = create_fmatrix(rows, columns, matrix, &frame);
	print_matrix(wow);

	// free the memory pool after its use
	pool_free(&frame);
}