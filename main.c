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

	float matrixa[3][4] = {{1.5f, 2.5f, 3.5f, 5.0f},
						   {4.0f, 5.0f, 6.0f, 5.0f}, 
						   {7.0f, 8.0f, 9.0f, 5.0f}};
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

	float matrixb[4][3] = {{1.0f, 1.0f, 1.0f},
						   {1.0f, 1.0f, 1.0f},
						   {1.0f, 1.0f, 1.0f},
						   {1.0f, 1.0f, 1.0f}};

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

	float matrixa[3][4] ={{1.0f, 2.0f, 3.0f, 4.0f},
						  {5.0f, 6.0f, 7.0f, 8.0f},
						  {9.0f, 10.0f, 11.0f, 12.0f}};

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

	float matA[3][3] = {{1.0f, 5.0f, -3.0f},
						{-1.0f, 0.0f, 4.0f},
						{12.0f, 5.0f, -1.0f}};
	fmatrix A = create_fmatrix(row33, col33, matA, &frame);

	float matB[3][3] = {{7.0f, -2.0f, 4.0f},
						{-1.0f, -1.0f, 3.0f},
						{1.0f, 2.0f, -3.0f}};
	fmatrix B = create_fmatrix(row33, col33, matB, &frame);

	printf("A:\n");
	print_fmatrix(A);

	printf("\nB:\n");
	print_fmatrix(B);

	float vecx[3][1] = {{1.0f},
						{-2.0f},
						{4.0f}};
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

	// initial alloc test
	{
		int size = 2;
		pool frame = create_pool(2 * sizeof(float));
		printf("pool created. size: %d\n", frame.size);

		float n1 = 3.2f;
		float* a = pool_alloc(&frame, &n1, sizeof(float));
	
		printf("allocated %f to pointer a", *a);

		float n2 = 12.1f;
		float* b = pool_alloc(&frame, &n2, sizeof(float));

		// test to make sure ptr on pool bumps properly
		printf("allocated n2 to b. stuff on frame:\n a: %f\n b: %f\n", *a, *b);

		// test basic free pool
		free_pool(&frame);
		a = NULL; b = NULL;
	}

	// test pool reallocation
	{
		int size = 1;
		pool frame = create_pool(size * sizeof(int));
		printf("size of first int pool: %d\n", frame.size);

		int x = 2;
		int* c = pool_alloc(&frame, &x, sizeof(int));

		int y = 3;
		int* d = pool_alloc(&frame, &y, sizeof(int));

		if (frame.next == NULL) {
			printf("something went wrong with pool realloc!\n");
			exit(1);
		}

		pool* other_frame = frame.next;
		printf("size of second pool: %d\n", other_frame->size);

		printf("allocated stuff: \n  c = %d\n  d = %d\n", *c, *d);

		// test free_pool on a LL of pools
		free_pool(&frame);
	}

	// test raw pool allocation
	{
		int size = 2;
		pool frame = create_pool(size * sizeof(int));

		int* a = raw_pool_alloc(&frame, sizeof(int));
		*a = 7;

		int* b = raw_pool_alloc(&frame, sizeof(int));
		*b = 6;

		printf("allocated stuff:\n  a = %d\n  b = %d", *a, *b);
		free_pool(&frame);
	}
}

void test_row_scale() {
	int count33 = 6;
	int row33 = 3;
	int col33 = 3;
	pool frame = create_pool((count33 * row33 * col33) * sizeof(float));

	float matA[3][3] = {{1.0f, 5.0f, -3.0f},
						{-1.0f, 0.0f, 4.0f},
						{12.0f, 5.0f, -1.0f}};
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

	float matA[3][3] = {{1.0f, 5.0f, -3.0f},
						{-1.0f, 0.0f, 4.0f},
						{12.0f, 5.0f, -1.0f}};
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

	float matA[3][3] = {{1.0f, 5.0f, -3.0f},
		{-1.0f, 0.0f, 4.0f},
		{12.0f, 2.0f, -1.0f}};
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

	float matA[3][3] = {{1.0f, 5.0f, -3.0f},
		{-1.0f, 0.0f, 4.0f},
		{12.0f, 5.0f, -1.0f}};
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

	float matrixa[3][3] = {{1.0f, 1.0f, 1.0f},
						   {3.0f, 3.0f, 3.0f},
						   {0.0f, 4.5f, -4.0f}};

	/*float matrixa[3][3] = {{1.0, 0.0, 3.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0}};*/

	pool frame = create_pool(row33 * col33 * count33 * sizeof(float));
	
	fmatrix A = create_fmatrix(row33, col33, matrixa, &frame);

	printf("\nA: \n");
	print_fmatrix(A);

	printf("\n|A| = %g\n", fmatrix_determinant(A, &frame));

	printf("A: \n");
	print_fmatrix(A);

	free_pool(&frame);
}

/*void run_function(fmatrix mat, pool* frame, (*func)(int n, ...) {
	
}*/

void run_normal_inverse(int r, int c, float** mat) {
	int count = 3; // one for the input, one for the copy of the input, and a third one for the inverse
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	fmatrix A = create_fmatrix(r, c, mat, &frame);
	printf("testing matrix:\n");
	print_fmatrix(A);

	printf("\ninverse:\n");

	fmatrix invA = fmatrix_inverse(A, &frame);

	if (invA.matrix == NULL){
		printf("matrix could not be inverted!\n");
		return;
	}

	print_fmatrix(invA);
	printf("\n");

	free_pool(&frame);
}

void run_transpose_inverse(int r, int c, float** mat) {
	int count = 3; // one for the input, one for the copy of the input, and a third one for the inverse
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	fmatrix A = create_fmatrix(r, c, mat, &frame);
	printf("testing matrix:\n");
	print_fmatrix(A);

	printf("Transpose: \n");
	fmatrix_transpose_in(&A);
	print_fmatrix(A);

	printf("\ninverse:\n");

	fmatrix invA = fmatrix_inverse(A, &frame);

	if (invA.matrix == NULL){
		printf("matrix could not be inverted!\n");
		return;
	}

	print_fmatrix(invA);
	printf("\n");

	free_pool(&frame);
}

void test_inverse() {
	// test transpose for each case as well

	// test a basic invertible matrix
	{
		printf("testing invertible matrix:\n");
		float A[3][3] = {{3.0f, 2.0f, 1.0f},
							{-1.0f, 0.0f, 5.0f},
							{2.0f, 2.0f, 2.0f}};
		run_normal_inverse(3, 3, A);
		run_transpose_inverse(3, 3, A);
	}

	// test a non square matrix
	{
		printf("testing non square matrix:\n");
		float B[3][2] = {{1.0f, 2.0f},
						 {3.0f, 0.0f},
						 {-3.2f, 4.2f}};
		run_normal_inverse(3, 2, B);
		run_transpose_inverse(3, 2, B);
	}

	// test a square, but not invertible, matrix
	{
		printf("testing non invertible matrix:\n");
		float C[3][3] = {{1.0f, 3.0f, -1.0f},
						  {-4.0f, 2.0f, 2.0f},
						  {-3.0f, 5.0f, 1.0f}};

		run_normal_inverse(3, 3, C);
		run_transpose_inverse(3, 3, C);
	}
}

void run_col_space(int r, int c, float* A) {
	int count = 3; // one for the input, one for the copy of the input
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nmatrix: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("result: \n");
	fmatrix result = fmatrix_col_space(mat, &frame);
	print_fmatrix(result);

	printf("\n result as an array in memory:\n");
	print_memory_layout(result);

	free_pool(&frame);
}

void run_col_space_transpose(int r, int c, float* A) {
	int count = 3; // one for the input, one for the copy of the input
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nmatrix: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("transpose: \n");
	fmatrix_transpose_in(&mat);
	print_fmatrix(mat);

	printf("result: \n");
	fmatrix result = fmatrix_col_space(mat, &frame);
	print_fmatrix(result);

	printf("\n result as an array in memory:\n");
	print_memory_layout(result);

	free_pool(&frame);
}

void test_col_space() {
	// test a basic invertible matrix
	{
		printf("testing invertible matrix:\n");
		float A[3][3] = {{ 3.0f, 2.0f, 1.0f},
						 {-1.0f, 0.0f, 5.0f},
						 { 2.0f, 2.0f, 2.0f}};
		run_col_space(3, 3, A);
	}

	// test a non invertible matrix
	{
		printf("\ntesting non invertible matrix and its transpose:\n");
		float B[3][4] = {	{ 3.0f, 2.0f, 5.0f, 1.0f},
							{-1.0f, 0.0f, 4.0f, 5.0f},
							{ 2.0f, 2.0f, 6.0f, 2.0f}};
		run_col_space(3, 4, B);

		run_col_space_transpose(3, 4, B);
	}

	// test a non invertible matrix that requires 2 col swaps
	{
		printf("\ntesting multiple swaps and transpose:\n");
		float B[3][5] = {	{ 3.0f, 2.0f, 5.0f, 0.0f, 1.0f},
							{-1.0f, 0.0f, 4.0f, 0.0f, 5.0f},
							{ 2.0f, 2.0f, 6.0f, 0.0f, 2.0f}};
		run_col_space(3, 5, B);

		run_col_space_transpose(3, 5, B);
	}

	// test the 0 matrix
	{
		printf("\ntesting a 0 matrix\n");
		float C[4][3] = {{0.0f, 0.0f, 0.0f},
						 {0.0f, 0.0f, 0.0f},
						 {0.0f, 0.0f, 0.0f},
						 {0.0f, 0.0f, 0.0f}};
		run_col_space(4, 3, C);
		run_col_space_transpose(4, 3, C);
	}
}

void run_row_space(int r, int c, float* A) {
	int count = 3; // one for the input, one for the copy of the input, one for the result
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nmatrix: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("result: \n");
	fmatrix result = fmatrix_row_space(mat, &frame);
	print_fmatrix(result);

	printf("\n result as an array in memory:\n");
	print_memory_layout(result);

	free_pool(&frame);
}

void run_row_space_transpose(int r, int c, float* A) {
	int count = 3; // one for the input, one for the copy of the input
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nmatrix: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("transpose: \n");
	fmatrix_transpose_in(&mat);
	print_fmatrix(mat);

	printf("result: \n");
	fmatrix result = fmatrix_row_space(mat, &frame);
	print_fmatrix(result);

	printf("\n result as an array in memory:\n");
	print_memory_layout(result);

	free_pool(&frame);
}

void test_row_space() {
	// test a basic invertible matrix
	{
		printf("testing invertible matrix:\n");
		float A[3][3] = {{ 3.0f, 2.0f, 1.0f},
			{-1.0f, 0.0f, 5.0f},
			{ 2.0f, 2.0f, 2.0f}};
		run_row_space(3, 3, A);
	}

	// test a non invertible matrix
	{
		printf("\ntesting non invertible matrix and its transpose:\n");
		float B[3][4] = {	{ 3.0f, 2.0f, 5.0f, 1.0f},
			{-1.0f, 0.0f, 4.0f, 5.0f},
			{ 2.0f, 2.0f, 6.0f, 2.0f}};
		run_row_space(3, 4, B);

		run_row_space_transpose(3, 4, B);
	}

	// test a non invertible matrix that requires 2 col swaps
	{
		printf("\ntesting multiple swaps and transpose:\n");
		float B[3][5] = {{ 3.0f, 2.0f, 5.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 4.0f, 0.0f, 5.0f},
			{ 2.0f, 2.0f, 6.0f, 0.0f, 2.0f}};
		run_row_space(3, 5, B);

		run_row_space_transpose(3, 5, B);
	}

	// test the 0 matrix
	{
		printf("\ntesting a 0 matrix\n");
		float C[4][3] = {{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f}};
		run_row_space(4, 3, C);
		run_row_space_transpose(4, 3, C);
	}
}

void run_LU(int r, int c, float* A) {
	int count = 6; // one for the input, one for each of PLU, then two extra temporary for multiplication test
	pool frame = create_pool(count * r * c * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nmatrix: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("frame pointer offset before: %td\n", (char*)frame.ptr - (char*)frame.start);

	/*printf("transpose:\n");
	fmatrix_transpose_in(&mat);
	print_fmatrix(mat);*/

	fmatrix PLU[3];
	if (fmatrix_LU_factorize(mat, PLU, &frame) == NULL) {
		printf("LU factorization for this matrix does not exist!\n");
		printf("frame pointer offset after: %td\n", (char*)frame.ptr - (char*)frame.start);
		free_pool(&frame);
		return;
	}

	printf("frame pointer offset after: %td\n", (char*)frame.ptr - (char*)frame.start);

	printf("P:\n");
	print_fmatrix(PLU[0]);
	//printf("%d\n", PLU[0].m);
	printf("\nL:\n");
	print_fmatrix(PLU[1]);
	//printf("%d\n", PLU[1].m);
	printf("\nU:\n");
	print_fmatrix(PLU[2]);

	printf("\nPA = LU -> A = P^tLU:\n");
	fmatrix_transpose_in(&PLU[0]);
	print_fmatrix(fmatrix_multiply(PLU[0], fmatrix_multiply(PLU[1], PLU[2], &frame), &frame));

	free_pool(&frame);
}

void test_LU() {
	// test a basic invertible matrix
	{
		printf("testing invertible matrix:\n");
		float A[2][2] = {{ 3.0f, 2.0f},
			{7.0f, 4.0f}};
		run_LU(2, 2, A);
	}

	// test an invertible matrix that requires a pivot
	{
		printf("testing a pivoting matrix:\n");
		float A[3][3] = {{ 0.0f, 2.0f, 1.0f},
			{1.0f, 1.0f, 0.0f}, 
			{2.0f, 1.0f, 1.0f}};
		run_LU(3, 3, A);
	}

	// test a rank-deficient matrix (has no LU factorization)
	{
		printf("testing rank deficient matrix:\n");
		float A[3][3] = {{ 0.0f, 2.0f, 1.0f},
			{0.0f, 0.0f, 0.0f}, 
			{2.0f, 1.0f, 1.0f}};
		run_LU(3, 3, A);
	}
}

void run_LU_solve(int r, int c, float* A, float* b) {
	int countmm = 4; // 1 for A, 3 for PLU 
	int countm1 = 5; // 1 for b, 2 for x and y, 1 for Pb, 1 for testing Ax, 1 for result
	pool frame = create_pool((countmm * r * c + countm1 * c) * sizeof(float));

	if (frame.start == NULL) {
		exit(1);
	}

	printf("\nA: \n");
	fmatrix mat = create_fmatrix(r, c, A, &frame);
	print_fmatrix(mat);

	printf("\nb: \n");
	fmatrix vec = create_fmatrix(c, 1, b, &frame);
	print_fmatrix(vec);

	fmatrix x = fmatrix_LU_solve(mat, vec, &frame);

	printf("\nx: \n");
	print_fmatrix(x);

	printf("test Ax = b:\n");
	print_fmatrix(fmatrix_multiply(mat, x, &frame));

	free_pool(&frame);
}

test_LU_solve() {
	// test a basic matrix and vector
	/*{
		printf("testing normal matrix:\n");
		float A[3][3] = {{1.0f, -3.0f, 0.0f}, 
			{0.0f, 1.0f, 3.0f}, 
			{2.0f, -10.0f, 2.0f}};
		float b[3][1] = {-5.0f, -1.0f, -20.0f};
		run_LU_solve(3, 3, A, b);
	}*/

	// test a matrix that requires permutations
	{
		printf("testing matrix that requires permutation:\n");
		float A[3][3] = {{0.0f, 1.0f, 3.0f}, 
			{2.0f, -10.0f, 2.0f}, 
			{1.0f, -3.0f, 0.0f}};
		float b[3][1] = {-1.0f, -20.0f, -5.0};
		run_LU_solve(3, 3, A, b);
	}
}

int main() {
	switch(15){
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
	case 11:
		test_inverse();
		break;
	case 12:
		test_col_space();
		break;
	case 13:
		test_row_space();
		break;
	case 14:
		test_LU();
		break;
	case 15:
		test_LU_solve();
		break;
	default:
		printf("no tests\n");
	}

	printf("\ndone");
}