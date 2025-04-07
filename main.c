#include "testing.h"

void test_transpose() {
	// 2 3x4 matrices
	int count = 2;
	int rows = 3, cols = 4;
	pool frame = create_pool(count * rows * cols * sizeof(float));

	float matrixa[3][4] = {{1.5, 2.5, 3.5, 5},
						   {4.0, 5.0, 6.0, 5}, 
						   {7.0, 8.0, 9.0, 5}};
	/*float matrixa[12] = {1.5, 2.5, 3.5, 5,
						 4.0, 5.0, 6.0, 5, 
						 7.0, 8.0, 9.0, 5};*/
	fmatrix A = create_fmatrix(rows, cols, matrixa[0], &frame);
	printf("A:\n");
	print_fmatrix(A);

	printf("A transpose:\n");
	//print_fmatrix(fmatrix_transpose(A, &frame));
	fmatrix copyA = fmatrix_transpose(A, &frame);
	print_properties(copyA);
	print_fmatrix(copyA);

	printf("\nin place:\n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);

	free_pool(&frame);
}

void test_add() {
	// 2 3x4 matrices, 1 4x3 matrix
	int count34 = 2;
	int rows34 = 3, cols34 = 4;

	int count43 = 2;
	int rows43 = 4, cols43 = 3;
	pool frame = create_pool((count34 * rows34 * cols34  +
							  count43 * rows43 * rows43) * 
							  sizeof(float));

	float matrixa[3][4] = {{1.5, 2.5, 3.5, 5},
						   {4.0, 5.0, 6.0, 5}, 
						   {7.0, 8.0, 9.0, 5}};
	fmatrix A = create_fmatrix(rows34, cols34, matrixa, &frame);
	printf("A:\n");
	print_fmatrix(A);

	printf("\nA + A:\n");
	print_fmatrix(fmatrix_add(A, A, &frame));

	printf("\ntranspose A + transpose A\n");
	fmatrix_transpose_in(&A);
	fmatrix copyA = fmatrix_add(A, A, &frame);
	//copyA.transpose = 1;
	print_properties(copyA);
	print_fmatrix(copyA);

	printf("\nB: \n");

	float matrixb[4][3] = {{1.0, 1.0, 1.0},
						   {1.0, 1.0, 1.0},
						   {1.0, 1.0, 1.0},
						   {1.0, 1.0, 1.0}};

	fmatrix B = create_fmatrix(rows43, cols43, matrixb, &frame);
	print_fmatrix(B);

	printf("\ntranspose A + B\n");

	fmatrix_add_in(A, B);
	print_fmatrix(A);

	free_pool(&frame);
}

void test_scale() {
	int count34 = 3;
	int row34 = 3;
	int col34 = 4;

	pool frame = create_pool((count34 * row34 * col34) * sizeof(float));

	float matrixa[3][4] ={{1.0, 2.0, 3.0, 4.0},
						  {5.0, 6.0, 7.0, 8.0},
						  {9.0, 10.0, 11.0, 12.0}};

	fmatrix A = create_fmatrix(row34, col34, matrixa, &frame);
	float scale = 2.0;

	printf("A: \n");
	print_fmatrix(A);

	printf("\nscale A by %g\n", scale);
	fmatrix copyA = A; // oops this is not doing what i wanted it to but the tests still work
	print_fmatrix(fmatrix_scale(A, scale, &frame));

	printf("\ninplace scale A by %g\n", scale);
	fmatrix_scale_in(A, scale);
	print_fmatrix(A);

	printf("\ntranspose and scale A by %g\n", scale);
	fmatrix_transpose_in(&copyA);
	print_fmatrix(fmatrix_scale(copyA, scale, &frame));

	printf("\ntranspose and inplace scale A by %g\n", scale);
	fmatrix_scale_in(copyA, scale);
	print_fmatrix(copyA);

	free_pool(&frame);
	
}

void test_multiplication() {
	int count33 = 2;
	int row33 = 3;
	int col33 = 3;
	int count31 = 4;
	int row31 = 3;
	int col31 = 1;

	pool frame = create_pool(((count33 * row33 * col33) +
							  (count31 * row31 * col31))*
							   sizeof(float));

	float matA[3][3] = {{1.0, 5.0, -3.0},
						{-1.0, 0.0, 4.0},
						{12.0, 5.0, -1.0}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	float matB[3][3] = {{7.0, -2.0, 4.0},
						{-1.0, -1.0, 3.0},
						{1.0, 2.0, -3.0}};
	fmatrix B = create_fmatrix(row33, col33, matB, &frame);

	printf("A:\n");
	print_fmatrix(A);

	printf("\nB:\n");
	print_fmatrix(B);

	float vecx[3][1] = {{1.0},
						{-2.0},
						{4.0}};
	fmatrix x = create_fmatrix(row31, col31, vecx, &frame);
	printf("\nx: \n");
	print_fmatrix(x);

	printf("\nAx: \n");
	print_fmatrix(fmatrix_multiply(A, x, &frame));

	/*printf("\nx^t\n");
	fmatrix_transpose_in(&x);
	print_fmatrix(x);

	printf("\nx * x^t: \n");
	fmatrix xn = fmatrix_transpose(x, &frame);
	print_fmatrix(fmatrix_multiply(xn, x, &frame));
	*/

	printf("\nx^t\n");
	fmatrix xtran = fmatrix_transpose(x, &frame);
	//fmatrix_transpose_in(&x);
	print_fmatrix(xtran);
	printf("\nis transpose: %d\n", xtran.transpose);

	printf("\nx * x^t: \n");
	print_fmatrix(fmatrix_multiply(x, xtran, &frame));
	print_fpool(&frame);
	free_pool(&frame);
}

void test_pool() {
	int size = 2;
	pool frame = create_pool(2 * sizeof(float));

	void* tp = frame.start;
	printf("start: %p\n end: %p\n", frame.start, frame.end);

	float* ap = raw_pool_alloc(&frame, sizeof(float));
	float* bp = raw_pool_alloc(&frame, sizeof(float));
	printf("frame.ptr before: %p\n", frame.ptr);

	pool_free_from(&frame, bp);

	printf("frame.ptr after: %p\n", frame.ptr);

	free_pool(&frame);
}

void test_row_scale() {
	int count33 = 6;
	int row33 = 3;
	int col33 = 3;
	pool frame = create_pool((count33 * row33 * col33) * sizeof(float));

	float matA[3][3] = {{1.0, 5.0, -3.0},
						{-1.0, 0.0, 4.0},
						{12.0, 5.0, -1.0}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	printf("A: \n");
	print_fmatrix(A);

	printf("\nA <- A^t: \n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);

	printf("\nR1 <- 1R1\n");
	fmatrix_row_scale_in(A, 0, 0.0);
	print_fmatrix(A);

	free_pool(&frame);
}

void test_row_swap() {
	int count33 = 6;
	int row33 = 3;
	int col33 = 3;
	pool frame = create_pool((count33 * row33 * col33) * sizeof(float));

	float matA[3][3] = {{1.0, 5.0, -3.0},
						{-1.0, 0.0, 4.0},
						{12.0, 5.0, -1.0}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	printf("A: \n");
	print_fmatrix(A);

	printf("\nA <- A^t: \n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);

	printf("\nR1 <- R2, R2 <- R1\n");
	//fmatrix_row_swap_in(A, 0, 1);
	print_fmatrix(fmatrix_row_swap(A, 0, 1, &frame));

	free_pool(&frame);
}

void test_row_sum() {
	int count33 = 6;
	int row33 = 3;
	int col33 = 3;
	pool frame = create_pool((count33 * row33 * col33) * sizeof(float));

	float matA[3][3] = {{1.0, 5.0, -3.0},
		{-1.0, 0.0, 4.0},
		{12.0, 2.0, -1.0}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	printf("A: \n");
	print_fmatrix(A);

	/*printf("\nA <- A^t: \n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);*/

	printf("\nR1 <- 2R1 + 3R2\n");
	//fmatrix_row_sum_in(A, 0, 2.0, 1, 3.0);
	print_fmatrix(fmatrix_row_sum(A, 0, 2.0, 1, 3.0, &frame));
	//print_fmatrix(A);

	free_pool(&frame);
}

void test_col_scale() {
	int count33 = 6;
	int row33 = 3;
	int col33 = 3;
	pool frame = create_pool((count33 * row33 * col33) * sizeof(float));

	float matA[3][3] = {{1.0, 5.0, -3.0},
		{-1.0, 0.0, 4.0},
		{12.0, 5.0, -1.0}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	printf("A: \n");
	print_fmatrix(A);

	printf("\nA <- A^t: \n");
	fmatrix At = fmatrix_transpose(A, &frame);
	print_fmatrix(At);

	printf("\nA: C1 <- 2C1\n");
	fmatrix_col_scale_in(A, 0, 2.0);
	print_fmatrix(A);

	free_pool(&frame);
}

void test_determinant() {
	int row33 = 3;
	int col33 = 3;
	int count33 = 1;

	float matrixa[3][3] = {{1.0, 1.0, 1.0},
						   {3.0, 3.0, 3.0},
						   {0.0, 4.5, -4.0}};

	/*float matrixa[3][3] = {{1.0, 0.0, 3.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0}};*/

	pool frame = create_pool(row33 * col33 * count33 * sizeof(float));
	
	fmatrix A = create_fmatrix(row33, col33, matrixa, &frame);

	printf("\nA: \n");
	print_fmatrix(A);

	printf("\n|A| = %g\n", fmatrix_determinant(A));

	printf("A: \n");
	print_fmatrix(A);
}

int main() {
	switch(10){
	case 1:
		test_transpose();
		break;
	case 2:
		test_add();
		break;
	case 3:
		test_scale();
		break;
	case 4:
		test_multiplication();
		break;
	case 5:
		test_pool();
		break;
	case 6:
		test_row_scale();
		break;
	case 7:
		test_row_swap();
		break;
	case 8:
		test_row_sum();
		break;
	case 9:
		test_col_scale();
		break;
	case 10:
		test_determinant();
		break;
	default:
		printf("\no tests");
	}

	printf("\ndone");
}