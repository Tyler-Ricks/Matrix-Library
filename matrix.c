#include "matrix.h"

// Checklist:
// (done) 1) finish refactoring and testing for transpose reading
// (done) 2) reorganize functions
// (done) 3) add better comments for each function
//        4) potentially add faster paths for non transpose matrices?
//        5) column operations (easy to do with transpose)
//        6) Look into sub expression overflow messages. Are they worth fixing? I think so

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

	if ((matrix = (float*)pool_alloc(frame, matrix, m * n * sizeof(float))) == NULL) {
		printf("pool allocation for matrix failed, returing empty matrix\n");
		return ERROR_FMATRIX;
	}
	// initially not a transpose, so field starts as 0
	return (fmatrix) {m, n, matrix, 0};
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

// fmatrix A2 = fmatrix_row_sum(A, 0, 3, 1, 0.5) // R1 <- 3R1 + 0.5R2
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
		return;
	}
	if (col2 >= mat.m || col2 < 0) {
		printf("col_swap error: \ncol2 %d out of bounds (make sure you are 0-indexed)\n", col2);
		return;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result; }

	fmatrix_col_swap_in(mat, col1, col2);
	return result;
}

void fmatrix_col_sum_in(fmatrix mat, int dest, float c1, int src, float c2) {
	if (dest >= mat.m || dest < 0) {
		printf("col_sum error: \dest col %d out of bounds (make sure you are 0-indexed)\n", dest);
		return;
	}
	if (src >= mat.m || src < 0) {
		printf("col_sum error: \src col %d out of bounds (make sure you are 0-indexed)\n", src);
		return;
	}

	fmatrix_transpose_in(&mat);
	fmatrix_row_sum_in(mat, dest, c1, src, c2);
	fmatrix_transpose_in(&mat);
}

fmatrix fmatrix_col_sum(fmatrix mat, int dest, float c1, int src, float c2, pool *frame) {
	if (dest >= mat.m || dest < 0) {
		printf("col_sum error: \dest col %d out of bounds (make sure you are 0-indexed)\n", dest);
		return;
	}
	if (src >= mat.m || src < 0) {
		printf("col_sum error: \src col %d out of bounds (make sure you are 0-indexed)\n", src);
		return;
	}

	fmatrix result = fmatrix_copy_alloc(mat, frame);
	if(!result.matrix){ return result;}

	fmatrix_col_sum_in(result, dest, c1, src, c2);
	return result;
}


// Determinants
// This is probably the second place where I can really hone down and optimize. For now, I'll just do basic
// cofactor expansion because it is recursive and that's cool

// Cofactor expansion is complicated to implement. I'll do it later. For now, I think getting det from triangulating a matrix is a 
// better idea. The stuff I figure out here will be useful for finding inverses as well.
// Maybe move the gaussian elimination stuff to its own function? many matrix things use it. Maybe have an "eliminate from column" thing

float fmatrix_triangle_determinant(fmatrix mat) {

	float track = 1.0; // row operations will influence this value, which we divide by at the end

	// gauss jordan (for each column, turn each element underneath the pivot into a 0 with row operations. Track the row operations in track
	float pivot, row_value;
	for (int i = 0; i < mat.n - 1; i++) {
		pivot = MATRIX_AT(mat, i, i);
		// check for pivot being 0. If it is, try to find a row to swap it with so we can do eliminations
		// very nested. Maybe move the procedure to finding a non 0 to its own function
		if (pivot == 0.0) { 
			for (int j = i; j < mat.m; j++) {
				if (MATRIX_AT(mat, i, j) != 0) {
					fmatrix_row_swap_in(mat, i, j);
					pivot = MATRIX_AT(mat, i, i);
					break;
				}
			}
			if(pivot == 0.0){ return 0.0; }
		}
		for (int j = i + 1; j < mat.m; j++) {
			printf("\nA: \n");
			print_fmatrix(mat);
			row_value = MATRIX_AT(mat, j, i);
			if (row_value == 0) { continue; }
			fmatrix_row_sum_in(mat, j, pivot, i, -row_value);
			track *= pivot;
		}
	}

	// gets the product of the diagonal elements
	float result = 1.0;
	for (int i = 0; i < mat.m; i++) {
		result *= MATRIX_AT(mat, i, i);
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

// checks if mat is non square, then calls fatrix_det (which recursively finds the determinant)

float fmatrix_determinant(fmatrix mat) {
	if (mat.m != mat.n) {
		printf("The determinant for a non square matrix (%d x %d) does not exist\n", mat.m, mat.n);
		exit(0); // okay do something else instead 
	}

	//return fmatrix_cofactor_expansion(mat, 0, 0, mat.m - 1, mat.n - 1);
	//return fmatrix_triangle_determinant(mat);

	float wow = fmatrix_triangle_determinant(mat);
	return wow;
}


