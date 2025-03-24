#include "testing.h"

void test_transpose() {
	// 2 3x4 matrices
	int count = 2;
	int rows = 3, cols = 4;
	pool frame = create_pool(count * rows * cols * sizeof(float));

	float matrixa[3][4] = {{1.5, 2.5, 3.5, 5},
						   {4.0, 5.0, 6.0, 5}, 
						   {7.0, 8.0, 9.0, 5}};
	fmatrix A = create_fmatrix(rows, cols, matrixa, &frame);
	printf("A:\n");
	print_fmatrix(A);

	printf("A transpose:\n");
	//print_fmatrix(fmatrix_transpose(A, &frame));
	fmatrix copyA = fmatrix_transpose(A, &frame);
	printf("\nA.m: %d, A.n: %d, A.tranpose: %d\n", copyA.m, copyA.n, copyA.transpose);
	print_fmatrix(copyA);

	printf("\nin place:\n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);

	free_pool(&frame);
}

int main() {

	test_transpose();
	/*int rows = 3;
	int columns = 3;
	// allocate a memory pool for allocated matrices
	pool frame = create_pool((rows * columns + 3 * 1 + 100) * sizeof(float));

	float matrixa[3][3] = {{1.0, 2.0, 3.0}, 
						   {4.0, 5.0, 6.0}, 
						   {7.0, 8.0, 9.0}};
	fmatrix A = create_fmatrix(rows, columns, matrixa, &frame);
	//print_matrix(wow);

	printf("%d", A.transpose);
	fmatrix_transpose_in(&A);
	printf("%d", A.transpose);*/


	/*float matrixb[3][4] = {{1.5, 2.5, 3.5, 5},
		{4.0, 5.0, 6.0, 5}, 
		{7.0, 8.0, 9.0, 5}};
	fmatrix B = create_fmatrix(3, 4, matrixb, &frame);

	printf("A:\n");
	print_fmatrix(A);

	printf("\nB:\n");
	print_fmatrix(B);

	printf("\ntranspose of B\n");
	//print_transpose(B);
	printf("\nbefore: m = %d, n = %d\n", B.m, B.n);
	fmatrix_transpose_in(&B);
	//print_transpose(B);
	printf("\nafter: m = %d, n = %d\n", B.m, B.n);
	print_fmatrix(B);

	printf("\n B[1][1] = %g", ATTEMPT_MATRIX_AT(B, 1, 1));

	// free the memory pool after its use
	free_pool(&frame);*/
}