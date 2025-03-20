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

void raw_print(fmatrix mat) {
	int i, j;
	for (i = 0; i < mat.m * mat.n; i++) {
		printf("%g ", mat.matrix[i]);
	}
	printf("\n");
}

int main() {
	int rows = 3;
	int columns = 2;
	pool frame = create_pool(rows * columns * sizeof(float));

	float matrix[3][2] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
	fmatrix wow = create_fmatrix(rows, columns, matrix, &frame);
	raw_print(wow);
}