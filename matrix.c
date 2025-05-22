#include "matrix.h"

// Checklist:
//        1) potentially add faster paths for non transpose matrices?
//		  2) extend LU factorization to non square matrices?
//		  3) make fmatrix_col_space more in place?
//		  4) Look into optimization, especially for multiplication and inverse
//		  5) Implement a bunch of stuff related to graphics (more specificity on this later)
//

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

// creates an m x n matrix with all elements set to 0.
fmatrix fmatrix_create_zero(int m, int n, pool* frame) {
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

// Utilities

// prints an input matrix in row major order.
//
// print_fmatrix(matA);
void print_fmatrix(fmatrix mat) {
	for (int i = 0; i < mat.m; i++) {
		for (int j = 0; j < mat.n; j++) {
			printf("%4.3f ", MATRIX_AT(mat, i, j));
		}
		printf("\n");
	}
}

// prints floats from a pool linearly
// used for debugging weird memory things, or tracking how transposes are stored
//
// printf("contents of frame:\n");
// print_pool(&frame);
void print_fpool(pool *frame) {
	for (int i = 0; i < (float*)frame->ptr - (float*)frame->start; i++) {
		printf("%g ", ((float*) frame->start)[i]);
	}
}

// prints the row and column count of mat, as well as if it's a transpose
//
// print_properties(matAt);
void print_properties(fmatrix mat) {
	printf("\nA.m: %d, A.n: %d, A.tranpose: %d\n", mat.m, mat.n, mat.transpose);
}

// simply prints a matrix as a an array.
// useful for making sure transpose reading is working properly
//
// print_as_array(matAt);
void print_as_array(fmatrix mat) {

	for(int i = 0; i < mat.m; i++){
		for (int j = 0; j < mat.n; j++) {
			printf("%g ", MATRIX_AT(mat, i, j));
		}
	}
	printf("\n");
}

void print_memory_layout(fmatrix mat) {
	int size = mat.m * mat.n;

	for (int i = 0; i < size; i++) {
		printf("%f ", mat.matrix[i]);
	}

	printf("\n");
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

// swaps the values of floats located at a and b
// used for a few row operations
//
// fswap(&mat[INDEX_AT(mat, 1, 2)], &mat[INDEX_AT(mat, 0, 2)]); 
void fswap(float *a, float *b) {
	float temp = *a;
	*a = *b;
	*b = temp;
}

// swaps the values of integers located at a and b
// literally only used for swapping m and n for transposing a matrix
//
// intswap(&matA.m, &matA.n);
void intswap(int* a, int* b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}


// from here on out, there are inplace versions of most functions. These do the same thing as their non inplace 
// counterparts, but they store their results in one of the inputs, avoiding extra memory allocation.
// Also, upon failure, an error message is printed, and no change is made to the inputs, rather than returning 
// an ERROR_FMATRIX.
// Keep in mind that many of the non inplace variants actually use the inplace version after copying one of
// their inputs.


// Basic matrix operations

// Adds two input matrices into matA, given that they have the same dimensions
// 
// fmatrix_add_in(A, B);
void fmatrix_add_in(fmatrix matA, fmatrix matB) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return;
	}
	for(int i = 0; i < matB.m; i++){
		for (int j = 0; j < matA.n; j++) {
			matA.matrix[INDEX_AT(matA, i, j)] += matB.matrix[INDEX_AT(matB, i, j)];
		}
	}
}

// fmatrix sumAB = fmatrix_add(A, B, &frame)
fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}

	float* matrix;
	if ((matrix = (float*)raw_pool_alloc(frame, matA.m * matA.n * sizeof(float))) == NULL) {
		printf("error while adding: pool allocation failure\n");
		return ERROR_FMATRIX;
	}

	fmatrix result = (fmatrix) {matA.m, matA.n, matrix};

	for (int i = 0; i < matA.m; i++) {
		for(int j = 0; j < matA.n; j++){
			result.matrix[INDEX_AT(result, i, j)] = MATRIX_AT(matA, i, j) + MATRIX_AT(matB, i, j);
		}
	}

	return result;
}

// subtracts values of matB from values of matA, given that they have the same dimensions
// 
// fmatrix_subtract_in(A, B);
void fmatrix_subtract_in(fmatrix matA, fmatrix matB) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while adding: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return;
	}
	for(int i = 0; i < matB.m; i++){
		for (int j = 0; j < matA.n; j++) {
			matA.matrix[INDEX_AT(matA, i, j)] -= matB.matrix[INDEX_AT(matB, i, j)];
		}
	}
}

// fmatrix diffAB = fmatrix_subtract_in(A, B);
fmatrix fmatrix_subtract(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.m != matB.m || matA.n != matB.n) {
		printf("error while subtracting: \ndimension mismatch: ");
		printf("matrix a: (%d x %d)  matrix b: (%d x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}

	float* matrix;
	if ((matrix = (float*)raw_pool_alloc(frame, matA.m * matA.n * sizeof(float))) == NULL) {
		printf("error while adding: pool allocation failure\n");
		return ERROR_FMATRIX;
	}

	fmatrix result = (fmatrix) {matA.m, matA.n, matrix};

	for (int i = 0; i < matA.m; i++) {
		for(int j = 0; j < matA.n; j++){
			result.matrix[INDEX_AT(result, i, j)] = MATRIX_AT(matA, i, j) - MATRIX_AT(matB, i, j);
		}
	}

	return result;
}

// scales all elements of mat by a float factor c
// if c = 1 or c = 0, it tries to save time by following a different proceedure
//
// fmatrix_scale_in(A, 2.5);
void fmatrix_scale_in(fmatrix mat, float c) {
	if(c == 1.0) { return; }
	int size = mat.m * mat.n;
	if(c == 0.0) { memset(mat.matrix, 0, size); return; }

	for(int i = 0; i < size; i++)
		mat.matrix[i] *= c;
}

// fmatrix scaledA = fmatrix_scale_in(A, 2.5);
fmatrix fmatrix_scale(fmatrix mat, float c, pool *frame) {
	float* matrix;
	int size = mat.m * mat.n;
	if ((matrix = (float*)raw_pool_alloc(frame, size * sizeof(float))) == NULL) {
		printf("error while scaling: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}
	fmatrix result = { mat.m, mat.n, matrix};

	for (int i = 0; i < mat.m; i++) {
		for(int j = 0; j < mat.n; j++){
			result.matrix[INDEX_AT(result, i, j)] = c * mat.matrix[INDEX_AT(mat, i, j)];
		}
	}

	return result;
}

// takes the sum of the products of elements of row i of matA times elements of col j of matB
// assumes that matA.m = matB.n
// used for fmatrix_multiply
// may change slightly and rename to dot product
//
// prod[INDEX_AT(prod, i, j)] = get_fmultiplied(matA, matB, i, j);
float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j) {
	float result = 0.0;

	for (int a = 0; a < matA.n; a++) {
		//printf("matA[%d][%d] = %g, matB[%d][%d] = %g\n", i, a, MATRIX_AT(matA, i, a), a, j, (MATRIX_AT(matB, a, j)));
		//print_fmatrix(matB);
		result += (MATRIX_AT(matA, i, a)) * (MATRIX_AT(matB, a, j));
	}

	return result;
}

// inplace matrix multiplcation is possible, but hardly practical for general use.
// maybe implement it for square matrices in the future
void fmatrix_multiply_in(fmatrix matA, fmatrix matB) {
	return; 
}

// multiplies matA and matB, given matA.n = matB.n. stores result in a new matrix in a pool
// very basic and brute force. 
//
// fmatrix AB = fmatrix_multiply(A, B, &frame);
fmatrix fmatrix_multiply(fmatrix matA, fmatrix matB, pool *frame) {
	if (matA.n != matB.m) {
		printf("error while multiplying: \ndimension mismatch: ");
		printf("matrix a: (%d x _%d_)  matrix b: (_%d_ x %d)\n", matA.m, matA.n, matB.m, matB.n);
		return ERROR_FMATRIX;
	}
	// new matrix has row count of A and col count of B
	float* matrix;
	if ((matrix = (float*)raw_pool_alloc(frame, matA.m * matB.n * sizeof(float))) == NULL) {
		printf("error while multiplying: \npool allocation failure\n");
		return ERROR_FMATRIX;
	}

	fmatrix result = (fmatrix){ matA.m, matB.n, matrix};

	for (int i = 0; i < matA.m; i++) {
		for(int j = 0; j < matB.n; j++){
			matrix[INDEX_AT(result, i, j)] = get_fmultiplied(matA, matB, i, j);
		}
	}

	return result;
}

// transposes mat in place
// literally just swaps mat.m and mat.n, then flips the transpose flag
// this works because the macros that read matrixes will check for the transpose flag and change
// how it reads if so.
// Regular matrix transpose is not that hard, so do it later, but still. This is constant time, 
// (at the cost of branching while reading) and it doesn't require much overhead.
// However, it could definitely lead to some problems if people don't know how transpose works 
// in this library. Hence why I want to have both implementations
//  
// the inplace version is also useful for implementing column operations later
//
// fmatrix_transpose_int(&A);
void fmatrix_transpose_in(fmatrix *mat) {
	// swaps m and n, and marks mat as a transpose
	intswap(&mat->m, &mat->n);
	mat->transpose = !mat->transpose;
}

// fmatrix At = fmatrix_transpose(A, &frame);
fmatrix fmatrix_transpose(fmatrix mat, pool* frame) {
	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if (!result.matrix) { return result; }

	fmatrix_transpose_in(&result);
	return result;
}


// Elementary row operations (make sure to 0 index row)

// multiplies all elements in row of mat by float c
// tries to save on time if c = 0 or c = 1
//
// fmatrix_row_scale_in(A, 0, 2.5); // scales elements of row 1 by 2.5
void fmatrix_row_scale_in(fmatrix mat, int row, float c) {
	if (row >= mat.m || row < 0) {
		printf("row_scale error: \nrow %d out of bounds (make sure you are 0-indexed)\n", row);
		return;
	}
	if (c == 1.0) { return; }
	if (c == 0.0 && !mat.transpose) { 
		memset(&mat.matrix[INDEX_AT(mat, row, 0)], 0, mat.n * sizeof(float)); 
		return; 
	}

	for (int i = 0; i < mat.n; i++) 
		mat.matrix[INDEX_AT(mat, row, i)] *= c;
}

// fmatrix scaleR1 = fmatrix_row_scale(A, 0, 2.5), &frame; // scales elements of row 1 by 2.5
fmatrix fmatrix_row_scale(fmatrix mat, int row, float c, pool *frame) {
	if (row >= mat.m || row < 0) {
		printf("row_scale error: \nrow %d out of bounds (make sure you are 0-indexed)\n", row);
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){return result;}

	fmatrix_row_scale_in(result, row, c);
	return result;
}

// swaps elements of row1 of mat with elements of row2
//
// fmatrix_row_swap_in(A, 0, 2); // swaps R1 and R2 of A
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

// fmatrix swapR12 = fmatrix_row_swap(A, 0, 2, &frame); // swaps R1 and R2 of A
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
	if(!result.matrix){return result;}

	fmatrix_row_swap_in(result, row1, row2);
	return result;
}

// elements of dest row of mat becomes c1 * dest added to c2 * elements of src row
// tries to save time if c1 or c2 equal 0
//
// fmatrix_row_sum_in(A, 0, 3, 1, 0.5) // R1 <- 3R1 + 0.5R2
void fmatrix_row_sum_in(fmatrix mat, int dest, float c1, int src, float c2) {
	if (dest >= mat.m || dest < 0) {
		printf("row_sum error: dest row %d out of bounds (make sure you are 0-indexed)\n", dest);
		return;
	}
	if (src >= mat.m || src < 0) {
		printf("row_sum error: src row %d out of bounds (make sure you are 0-indexed)\n", src);
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

// fmatrix A2 = fmatrix_row_sum(A, 0, 3, 1, 0.5) // R1 <- 3R1 + 0.5R2
fmatrix fmatrix_row_sum(fmatrix mat, int dest, float c1, int src, float c2, pool* frame) {
	if (dest >= mat.m || dest < 0) {
		printf("row_sum error: dest row %d out of bounds (make sure you are 0-indexed)\n", dest);
		return (fmatrix){ 0, 0, NULL};
	}
	if (src >= mat.m || src < 0) {
		printf("row_sum error: src row %d out of bounds (make sure you are 0-indexed)\n", src);
		return (fmatrix){ 0, 0, NULL};
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	fmatrix_row_sum_in(result, dest, c1, src, c2);
	return result;
}


// elementary column operations
// This are all the "easy" methods. They transpose the input matrix, then run their respective row operation on
// it, then transpose it back. I'll implement direct methods later, but I wanted to demonstrate how efficient 
// transpose could be

// scales elements of a column by a factor of c
void fmatrix_col_scale_in(fmatrix mat, int col, float c) {
	if (col >= mat.n || col < 0) {
		printf("col_scale error: \ncol %d out of bounds (make sure you are 0-indexed)\n", col);
		return;
	}

	fmatrix_transpose_in(&mat);
	fmatrix_row_scale_in(mat, col, c);
	fmatrix_transpose_in(&mat);
}

fmatrix fmatrix_col_scale(fmatrix mat, int col, float c, pool* frame) {
	if (col >= mat.n || col < 0) {
		printf("col_scale error: \ncol %d out of bounds (make sure you are 0-indexed)\n", col);
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	fmatrix_col_scale_in(mat, col, c);
	return result;
}

void fmatrix_col_swap_in(fmatrix mat, int col1, int col2) {
	if (col1 >= mat.m || col1 < 0) {
		printf("col_swap error: \ncol1 %d out of bounds (make sure you are 0-indexed)\n", col1);
		return;
	}
	if (col2 >= mat.m || col2 < 0) {
		printf("col_swap error: \ncol2 %d out of bounds (make sure you are 0-indexed)\n", col2);
		return;
	}

	if(col1 == col2){ return; } // no change necessary

	fmatrix_transpose_in(&mat);
	fmatrix_row_swap_in(mat, col1, col2);
	fmatrix_transpose_in(&mat);
}

fmatrix fmatrix_col_swap(fmatrix mat, int col1, int col2, pool* frame) {
	if (col1 >= mat.m || col1 < 0) {
		printf("col_swap error: \ncol1 %d out of bounds (make sure you are 0-indexed)\n", col1);
		return ERROR_FMATRIX;
	}
	if (col2 >= mat.m || col2 < 0) {
		printf("col_swap error: \ncol2 %d out of bounds (make sure you are 0-indexed)\n", col2);
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	fmatrix_col_swap_in(mat, col1, col2);
	return result;
}

void fmatrix_col_sum_in(fmatrix mat, int dest, float c1, int src, float c2) {
	if (dest >= mat.m || dest < 0) {
		printf("col_sum error: \ndest col %d out of bounds (make sure you are 0-indexed)\n", dest);
		return;
	}
	if (src >= mat.m || src < 0) {
		printf("col_sum error: \nsrc col %d out of bounds (make sure you are 0-indexed)\n", src);
		return;
	}

	fmatrix_transpose_in(&mat);
	fmatrix_row_sum_in(mat, dest, c1, src, c2);
	fmatrix_transpose_in(&mat);
}

fmatrix fmatrix_col_sum(fmatrix mat, int dest, float c1, int src, float c2, pool *frame) {
	if (dest >= mat.m || dest < 0) {
		printf("col_sum error: \ndest col %d out of bounds (make sure you are 0-indexed)\n", dest);
		return ERROR_FMATRIX;
	}
	if (src >= mat.m || src < 0) {
		printf("col_sum error: \nsrc col %d out of bounds (make sure you are 0-indexed)\n", src);
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result;}

	fmatrix_col_sum_in(result, dest, c1, src, c2);
	return result;
}


// Determinants
// This is probably the second place where I can really hone down and optimize. For now, I'll just do basic
// cofactor expansion because it is recursive and that's cool


// finds a row to swap a 0 pivot with. returns -1 if none is found
// used in functions that use gaussian elimination, such as fmatrix_triangle_determinant
int find_pivot_row(fmatrix mat, int pivot_row, int col) {
	for (int j = pivot_row; j < mat.m; j++) {
		if (MATRIX_AT(mat, j, col) != 0) { return j; }
	}
	return -1;
}

// Cofactor expansion is complicated to implement. I'll do it later. For now, I think getting det from triangulating a matrix is a 
// better idea. The stuff I figure out here will be useful for finding inverses as well.
// Maybe move the gaussian elimination stuff to its own function? many matrix things use it. Maybe have an "eliminate from column" thing

// calculates determinant of a square matrix by triangulating it.
// copies the matrix to do operations to for now. It seems like you could do this lazily without copying or editing the matrix at all,
// but worry about that later
// Assumes that mat is a square matrix
float fmatrix_triangle_determinant(fmatrix mat, pool *frame) {

	fmatrix temp_mat = fmatrix_copy_alloc(mat, frame); // allocate a temporary copy of the input mat

	float track = 1.0; // row operations will influence this value, which we divide by at the end

	// gauss jordan (for each column, turn each element underneath the pivot into a 0 with row operations. Track the row operations in track
	int pivot_row;
	float pivot, row_value, result = 1.0;
	for (int i = 0; i < temp_mat.n; i++) {
		pivot_row = find_pivot_row(temp_mat, i, i);							// find a pivot in col i
		if(pivot_row == -1) { result = 0; break; }							// if none is found, det is 0
		if(pivot_row != i){ fmatrix_row_swap_in(temp_mat, i, pivot_row); }  // if pivot is not at row i, swap it in
		pivot = MATRIX_AT(temp_mat, i, i);

		// now that pivot is found, eliminate elements below it
		for (int j = i + 1; j < temp_mat.m; j++) {
			//printf("\nA: (pivot is %g)\n", pivot);
			//print_fmatrix(temp_mat);
			row_value = MATRIX_AT(temp_mat, j, i);
			if (row_value == 0.0) { continue; } // if element is already 0, don't eliminate
			fmatrix_row_sum_in(temp_mat, j, pivot, i, -row_value);
			track *= pivot;
		}
		result *= pivot; // multiply the found diagonal into result
	}

	// free the temporary metrix from the pool
	void* check = pool_free_from(frame, temp_mat.matrix);
	if (check == NULL) {
		printf("Failed to free temporary copy matrix from triangle determinant\n");
		return -1;
	}

	return result / track;
}

// takes a matrix, its upper and low row index, and its upper and lower column index. These indices bound the matrix we are finidng
// the determinant of. (inclusive) This behaves similarly to how cofactor expansion works normally. 
// Just like in cofactor expansion, we choose a row or column, then multiply each element of it with the
// determinant of the minor leftover after excluding that index row and column. Then, we sum all these 
// results.
// Oh yeah, don't forget to multiply cofactors by 1 or -1 :)

float fmatrix_cofactor_expansion(fmatrix mat, int lr, int lc, int ur, int uc) {
	if (ur - lr == 1) { // 2x2 matrix
		return (MATRIX_AT(mat, lr, lc) * MATRIX_AT(mat, ur, uc)) - (MATRIX_AT(mat, lr, uc) * MATRIX_AT(mat, ur, lc));
	}
	printf("lr: %d lc: %d ur: %d uc: %d\n", lr, lc, ur, uc);
	// iterate through the first column. Each element is a cofactor term
	for (int i = lr; i < ur; i++) {
		//printf("%g \n", MATRIX_AT(mat, i, lc));
	}
	printf("\n%g", MATRIX_AT(mat, 0, lc) * fmatrix_cofactor_expansion(mat, lr + 1, lc + 1, ur, uc));

	return(0.0);
}

// checks if mat is non square, then calls whichever determinent function to use

float fmatrix_determinant(fmatrix mat, pool *frame) {
	if (mat.m != mat.n) {
		printf("The determinant for a non square matrix (%d x %d) does not exist\n", mat.m, mat.n);
		exit(0); // okay do something else instead 
	}

	//return fmatrix_cofactor_expansion(mat, 0, 0, mat.m - 1, mat.n - 1);
	//return fmatrix_triangle_determinant(mat);
	return fmatrix_triangle_determinant(mat, frame);
}


// matrix inversion stuff

// finds and returns the inverse of mat. 
// returns ERROR_FMATRIX if an mat is not invertible
fmatrix fmatrix_inverse(fmatrix mat, pool* frame) {
	if (mat.m != mat.n) { return ERROR_FMATRIX; }

	fmatrix result = fmatrix_create_identity(mat.m, mat.n, frame);	// initialize an identity matrix
	fmatrix mat_copy = fmatrix_copy_alloc(mat, frame);				// for not modifying the input matrix. allocate second to free later

	int pivot_row;
	float row_value;												// for checking if we even need to make an elimination for an element
	for (int i = 0; i < mat_copy.n; i++) {							// for each column, iterate and eliminate elements below pivots
		pivot_row = find_pivot_row(mat_copy, i, i);
		if(pivot_row == -1) { return ERROR_FMATRIX; }				// no pivot was found, so mat is not invertible
		if(pivot_row != i) { 
			fmatrix_row_swap_in(mat_copy, i, pivot_row);			// swap the located pivot to the proper row
			fmatrix_row_swap_in(result, i, pivot_row);				// do it for the result matrix as well
		}

		float normalize = 1.0f / MATRIX_AT(mat_copy, i, i);
		fmatrix_row_scale_in(mat_copy, i, normalize);				// set the pivot to 1 by scaling its row
		fmatrix_row_scale_in(result, i, normalize);
		
		// for each row, eliminate elements under the pivot
		for (int j = 0; j < mat_copy.m; j++) {
			if(i == j) { continue; }								// don't eliminate the pivot
			row_value = MATRIX_AT(mat_copy, j, i);
			if(row_value == 0){ continue; }							// no elimination necessary
			fmatrix_row_sum_in(mat_copy, j, 1.0f, i, -row_value);	// eliminate the element
			fmatrix_row_sum_in(result,	 j, 1.0f, i, -row_value);
		}
	}

	// free the temporary matrix from the pool
	void* check = pool_free_from(frame, mat_copy.matrix);
	if (check == NULL) {
		printf("Failed to free temporary copy matrix from triangle determinant\n");
		return ERROR_FMATRIX;
	}

	return(result);
}


// functions for finding basis for the 4 spaces (row/column space, null/left null space)

// finds and returns a basis for the column space of mat
// uses gaussian elimination to find the pivot columns
// as we iterate through columns, "organize" the matrix s.t:
//	-pivot columns are on the left
//	-free columns are on the right
// The goal is to then edit the copied matrix to only include the vectors that form a basis for the null space
// However, even if I swap free columns to the right, they show up in memory in row major order. 
// So, I can't just use pool_free_from starting from the first free column.
// I could potentially write a proper in place transpose function, then use free from, then edit the fmatrix dimensions, then transpose back
// But there could be an easier and more efficient way
// For now, I am allocating another fmatrix on the pool, putting the pivot columns into it, then returning that
// you could allocate room for the result fmatrix, then the copy, and then at the end, free from the start of unused columns in result?
// I would rather work towards doing it all inside the copy matrix
fmatrix fmatrix_col_space(fmatrix mat, pool* frame) {
	fmatrix mat_cpy = fmatrix_copy_alloc(mat, frame);

	int pivot_row;					// the row number of a located pivot
	int check_row = 0;				// indicates which row to swap with the located pivot
	int swap_col = -1;				// indicates index of a column where no pivot was located. If column is found to have a pivot, swap it with this one.
	int rank = 0;					// counts the number of pivot columns
	float pivot_value, row_value;
	//float pivot_value;

	// iterate through each column. If a pivot is found, eliminate all values below it, then add the column to our column space. 
	//  If not, check the next column for a pivot, without increasing the row at which to check for that pivot
	for (int i = 0; i < mat_cpy.n; i++) {
		pivot_row = find_pivot_row(mat_cpy, check_row, i);
		if (pivot_row == -1) {						// if no pivot is found, then the column is not part of the col space
			if(swap_col == -1) { swap_col = i; }	// if no free column has been marked for swapping, mark this one.
			continue; 
		} 

		if (pivot_row != check_row) { fmatrix_row_swap_in(mat_cpy, pivot_row, check_row); }
		pivot_value = MATRIX_AT(mat_cpy, check_row, i);

		// eliminate all elements under the pivot
		for (int j = check_row + 1; j < mat_cpy.m; j++) {
			row_value = MATRIX_AT(mat_cpy, j, i);
			if (row_value == 0.0) { continue; } // if element is already 0, don't eliminate
			fmatrix_row_sum_in(mat_cpy, j, pivot_value, pivot_row, -row_value); // eliminate the element
			//print_fmatrix(mat_cpy);
		}

		check_row++;	// increment which row to check for pivots
		rank++;			// another free column has been found, so increase the rank.

		// if a marked free column exists for swapping, swap with the located pivot
		if(swap_col == -1) { continue; }
		
		fmatrix_col_swap_in(mat_cpy, swap_col, i);		// swap the column
		swap_col++;										// increment which column to swap.
	}

	// if the rank is 0 (mat is the 0 matrix), then return {0}
	// The span of the columns of a 0 matrix is just 0
	if (rank == 0) {
		float zero[1][1] = {{0.0f}};
		fmatrix result = create_fmatrix(1, 1, zero, frame);
		return result;
	}

	// allocate another fmatrix, then store the pivot columns inside it. 
	fmatrix result = fmatrix_ncol_copy_alloc(mat_cpy, rank, frame);

	return result;
}


// row space of A is equal to the col space of the transpose of A
// seems to fail when passed already transposed matrices
fmatrix fmatrix_row_space(fmatrix mat, pool* frame) {
	fmatrix_transpose_in(&mat);
	fmatrix result = fmatrix_col_space(mat, frame);
	fmatrix_transpose_in(&result);
	//print_fmatrix(result);
	fmatrix_transpose_in(&mat);
	return result;
}


// Factorizations and Decompositions
// These functions take in a matrix input, but need to return multiple matrices that make up the respective factorization/decompositon
// Each factorization has a defined number of matrices that make up its factorization, so for now, convention is to simply return an array of fmatrix pointers


// Technically, this function does PLU factorization
// PA = LU such that L is lower triangular, U is upper triangular, and P is a permutation matrix
// A permutation matrix is simply an identity matrix that has undergone row swaps. In this case, the row swaps we do correspond to the row
// swaps required to get A to be able to be row reduced to an upper triangular matrix. For matrices that need no row swaps, P is just identity
// 
// This function takes in mat, and populates an fmatrix array, with the first element being P, the second being L and the third being U
// Works for matrices that require partial pivoting, but not non-square matrices FOR NOW
// In the case that no LU factorization exists, (mat is "rank-deficient") free data from pool starting from U.matrix, then return NULL.
// Usage: This one requires you to pass in a declared fmatrix array:
// fmatrix PLU[3];
// if(fmatrix_LU_factorize(A, PLU, &frame) == NULL){
//     printf("LU factorization for A does not exist");
// }
fmatrix* fmatrix_LU_factorize(fmatrix mat, fmatrix PLU[3], pool* frame) {
	if (mat.m != mat.n) { return NULL; } // does not handle rectangular matrices for now

	// initialize P, L, and U
	fmatrix P, L, U;
	if((P = fmatrix_create_identity(mat.m, mat.n, frame)).matrix == NULL){ return NULL; }
	if((L = fmatrix_create_identity(mat.m, mat.n, frame)).matrix == NULL){ return NULL; }
	if((U = fmatrix_copy_alloc(mat, frame)).matrix == NULL){ return NULL; }

	// row reduce U into an upper triangular matrix
	int pivot_row;
	float pivot_value;
	for (int i = 0; i < U.n; i++) {
		pivot_row = find_pivot_row(U, i, i);
		if (pivot_row == -1) {						// if no pivot row is found, mat is rank-deficient
			pool_free_from(frame, P.matrix);
			return NULL;
		}
		if (pivot_row != i) {						// pivot row is found, but requires a pivot (row swap)
			fmatrix_row_swap_in(U, i, pivot_row);	// track row swaps in the permutation/pivot matrix P
			fmatrix_row_swap_in(P, i, pivot_row);
		}
		pivot_value = MATRIX_AT(U, i, i);

		// eliminate lower elements
		for (int j = i + 1; j < U.m; j++) {
			float row_value = MATRIX_AT(U, j, i);
			if(row_value == 0){ continue; } 

			float k = (row_value / pivot_value);
			fmatrix_row_sum_in(U, j, 1, i, -k);
			L.matrix[INDEX_AT(L, j, i)] = k;		// track eliminations in L
		}
	}
	// populate the passed in array
	PLU[0] = P;
	PLU[1] = L;
	PLU[2] = U;

	return PLU;
}

// You can solve a system of n equations in n variables quickly using LU factorization. 
// let Ax = b (A and b are given, A is m x m, and b = m x 1)
// let A = LU (where L is lower triangular, and U is upper triangular)
// so: LUx = b
// let y = Ux
// so: Ly = b
// L is lower triangular, so it is easy to solve for y in Ly = b
// Ux = y
// U is upper triangular, and y is now known, so it is easy to solve for x
// 
// Takes in m x m matrix A, m x 1 vector b, then returns an m x 1 vector x
// If LU_factorize returns NULL, then handle that somehow.
// If this happens, then there are either infinite solutions, or zero. Maybe return matrices that indicate this?
// They need to be seperate from ERROR_FMATRIX, which should only return on actual errors
fmatrix fmatrix_LU_solve(fmatrix A, fmatrix b, pool* frame) {
	if (A.m != A.n) {
		printf("Solving a system requires a square matrix (for now)\n");
		return ERROR_FMATRIX;
	}
	if (b.m != A.m && b.n != 1) {
		printf("Solving a system requires b to be m x 1, where m is A.m\n");
		return ERROR_FMATRIX;
	}

	// get the PLU factorization of A, and check if it even exists. 
	// for now, just return, but in the future actually handle the case
	fmatrix PLU[3];
	if (fmatrix_LU_factorize(A, PLU, frame) == NULL) { return A; }
	fmatrix P = PLU[0];
	fmatrix L = PLU[1];
	fmatrix U = PLU[2];

	// allocate space for x and y, but do x first so we can free up y at the end
	fmatrix x = fmatrix_create_zero(A.m, 1, frame);
	if (x.matrix == NULL) {return ERROR_FMATRIX; }
	fmatrix y = fmatrix_create_zero(A.m, 1, frame);
	if (y.matrix == NULL) { 
		pool_free_from(frame, x.matrix);
		return ERROR_FMATRIX; 
	}

	// also allocate a copy of p, so we can permute it (LUx = Pb)
	// I wonder if we can do this lazily, just by accessing the correct element of b
	//fmatrix_transpose_in(&P);
	/*print_fmatrix(fmatrix_multiply(L, U, frame));
	printf("P:\n");
	print_fmatrix(P);*/
	fmatrix Pb = fmatrix_multiply(P, b, frame);
	//fmatrix Pb = fmatrix_copy_alloc(b, frame);

	/*printf("\nL\n");
	print_fmatrix(L);
	printf("\nU\n");
	print_fmatrix(U);
	printf("\nPb\n");
	print_fmatrix(Pb);*/

	// Ly = Pb, solve for y
	for (int r = 0; r < L.m; r++) {
		// iterate through the current row until you reach the diagonal 
		float b_r = Pb.matrix[r]; // use elements of b to determine corresponding elements of y
		for (int c = 0; c < r; c++) {
			b_r -= (y.matrix[c] * MATRIX_AT(L, r, c));
			//printf("uh: %f * %f = %f\n", y.matrix[c], MATRIX_AT(L, r, c), y.matrix[c] * MATRIX_AT(L, r, c));
		}
		y.matrix[r] = b_r; // For now, diagonal elements of L are 1.0. Make sure to change this line if that changes
	}

	//printf("\ntest y:\n");
	//print_fmatrix(y);

	// Ux = y, solve for x (iterate from the bottom)
	for (int r = U.m - 1; r >= 0; r--) {
		// iterate backwards through the current row until you reach the diagonal 
		float y_r = y.matrix[r]; // use elements of y to determine corresponding elements of x
		for (int c = U.n - 1; c > r; c--) {
			y_r -= (x.matrix[c] * MATRIX_AT(U, r, c));
			//printf("uh: %f * %f = %f\n", y.matrix[c], MATRIX_AT(L, r, c), y.matrix[c] * MATRIX_AT(L, r, c));
		}
		x.matrix[r] = y_r / MATRIX_AT(U, r, r); 
	}

	/*printf("\ntest x:\n");
	print_fmatrix(x);*/

	// free up y, which won't be used anymore
	pool_free_from(frame, y.matrix);

	return x;
}
