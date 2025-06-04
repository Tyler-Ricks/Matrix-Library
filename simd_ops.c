#include "matrix.h"

// loads 4 contiguous floats in A's matrix, starting from i
// DOES NOT CHECK IF THIS WOULD STEP OUT OF MATRIX BOUNDS
// returns 
__m128 simd_load_cont(fmatrix A, int i) {
	return _mm_loadu_ps(&A.matrix[i]);
}

__m128 simd_load_transpose(fmatrix A, int i) {
	return _mm_set_ps(	A.matrix[ARRAY_INDEX(A, i + 3)],
		A.matrix[ARRAY_INDEX(A, i + 2)],
		A.matrix[ARRAY_INDEX(A, i + 1)],
		A.matrix[ARRAY_INDEX(A, i)]);
}

// adds two 4x4 matrices, returns a new matrix with the result. 
// if in CMO, adds all 4 elements of a given column at once with SSE SIMD
// stores the result column in the new matrix
// if in RMO, it adds by row instead, as rows are stored contiguously instead of columns
#ifdef CMO
fmatrix simd_44_add(fmatrix A, fmatrix B, pool* frame) {
	if (A.m != 4 || A.n != 4) {
		printf("Error in simd44_add: must pass a 4x4 matrix into 1st parameter\n");
		return	ERROR_FMATRIX;
	}
	if (B.m != 4 || B.n != 4) {
		printf("Error in simd44_add: must pass a 4x4 matrix into 2nd parameter\n");
		return	ERROR_FMATRIX;
	}

	float C[16];
	fmatrix result = create_fmatrix(4, 4, C, frame);
	if(!result.matrix){ return result; } // fmatrix allocation failed

	// iterate through columns of A and B, accounting for stride
	for (int i = 0; i < 4; i++ ) {
		__m128 colA = _mm_set_ps(	MATRIX_AT(A, 3, i),
			MATRIX_AT(A, 2, i),
			MATRIX_AT(A, 1, i),
			MATRIX_AT(A, 0, i));
		//printf("colA: %f, %f, %f, %f\n", MATRIX_AT(A, 3, i), MATRIX_AT(A, 2, i), MATRIX_AT(A, 1, i), MATRIX_AT(A, 0, i));

		__m128 colB = _mm_set_ps(	MATRIX_AT(B, 3, i),
			MATRIX_AT(B, 2, i),
			MATRIX_AT(B, 1, i),
			MATRIX_AT(B, 0, i));
		//printf("colB: %f, %f, %f, %f\n\n", MATRIX_AT(B, 3, i), MATRIX_AT(B, 2, i), MATRIX_AT(B, 1, i), MATRIX_AT(B, 0, i));
		__m128 colC = _mm_add_ps(colA, colB);
		_mm_storeu_ps(&result.matrix[i * 4], colC);	// store the contents of c into thecurrent column location of C
	}

	return result;
}


#else
fmatrix simd_44_add(fmatrix A, fmatrix B, pool* frame) {
	if (A.m != 4 || A.n != 4) {
		printf("Error in simd44_add: must pass a 4x4 matrix into 1st parameter\n");
		return	ERROR_FMATRIX;
	}
	if (B.m != 4 || B.n != 4) {
		printf("Error in simd44_add: must pass a 4x4 matrix into 2nd parameter\n");
		return	ERROR_FMATRIX;
	}

	float C[16];
	fmatrix result = create_fmatrix(4, 4, C, frame);
	if(!result.matrix){ return result; } // fmatrix allocation failed

	// iterate through columns of A and B, accounting for stride
	for (int i = 0; i < 4; i++ ) {
		__m128 colA = _mm_set_ps(	MATRIX_AT(A, i, 3),
			MATRIX_AT(A, i, 2),
			MATRIX_AT(A, i, 1),
			MATRIX_AT(A, i, 0));
		//printf("colA: %f, %f, %f, %f\n", MATRIX_AT(A, 3, i), MATRIX_AT(A, 2, i), MATRIX_AT(A, 1, i), MATRIX_AT(A, 0, i));

		__m128 colB = _mm_set_ps(	MATRIX_AT(B, i, 3),
			MATRIX_AT(B, i, 2),
			MATRIX_AT(B, i, 1),
			MATRIX_AT(B, i, 0));
		//printf("colB: %f, %f, %f, %f\n\n", MATRIX_AT(B, 3, i), MATRIX_AT(B, 2, i), MATRIX_AT(B, 1, i), MATRIX_AT(B, 0, i));
		__m128 colC = _mm_add_ps(colA, colB);
		_mm_storeu_ps(&result.matrix[i * 4], colC);	// store the contents of c into thecurrent column location of C
	}

	return result;
}

#endif 

fmatrix fmatrix_simd_add(fmatrix A, fmatrix B, pool* frame) {
	if (A.m != B.m || A.n != B.n) {
		printf("Error in simd_add: dimension mismatch for input matrices\n");
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_create_raw(A.m, A.n, frame);
	if(!result.matrix){ return result; }

	// determine if we can load matrix memory contiguously into simd registers (contiguous loading takes less instructions)
	__m128 (*load_A)(fmatrix, int);
	__m128 (*load_B)(fmatrix, int);
	if (A.transpose == B.transpose) { // linear memory access lines up for both matrices
		load_A = &simd_load_cont;
		load_B = &simd_load_cont;
	}
	else if (A.transpose == 1) {	
		load_A = &simd_load_transpose;
		load_B = &simd_load_cont;
	}
	else {
		load_A = &simd_load_cont;
		load_B = &simd_load_transpose;
	}

	int size = A.m * A.n;
	int bound = size - (size % 4);
	int i = 0;
	for (i; i < bound; i += 4) {
		__m128 colA = load_A(A, i);
		//printf("colA: %f, %f, %f, %f\n", A.matrix[ARRAY_INDEX(A, i)], A.matrix[ARRAY_INDEX(A, i + 1)], A.matrix[ARRAY_INDEX(A, i + 2)], A.matrix[ARRAY_INDEX(A, i + 3)]);

		__m128 colB = load_B(B, i);
		//printf("colB: %f, %f, %f, %f\n\n", B.matrix[ARRAY_INDEX(B, i)], B.matrix[ARRAY_INDEX(B, i + 1)], B.matrix[ARRAY_INDEX(B, i + 2)], B.matrix[ARRAY_INDEX(B, i + 3)]);
		//printf("next\n");
		__m128 colC = _mm_add_ps(colA, colB);
		_mm_storeu_ps(&result.matrix[i], colC);	// store the contents of c into the current column location of C
	}

	// add any elements that don't fit into SIMD slots (up to 3)
	for (i; i < size; i++) {
		result.matrix[i] = A.matrix[ARRAY_INDEX(A, i)] + B.matrix[ARRAY_INDEX(B, i)];
	}

	return result;
}

// same thing as fmatrix_simd_add, but does subtraction instead of addition.
fmatrix fmatrix_simd_subtract(fmatrix A, fmatrix B, pool* frame) {
	if (A.m != B.m || A.n != B.n) {
		printf("Error in simd_add: dimension mismatch for input matrices\n");
		return ERROR_FMATRIX;
	}

	fmatrix result = fmatrix_create_raw(A.m, A.n, frame);
	if(!result.matrix){ return result; }

	// determine if we can load matrix memory contiguously into simd registers
	// if both matrices are lazy transpose, then contiguous access of each maps properly
	__m128 (*load_A)(fmatrix, int);
	__m128 (*load_B)(fmatrix, int);
	if (A.transpose == B.transpose) {
		load_A = &simd_load_cont;
		load_B = &simd_load_cont;
	}
	else if (A.transpose == 1) {	
		load_A = &simd_load_transpose;
		load_B = &simd_load_cont;
	}
	else {
		load_A = &simd_load_cont;
		load_B = &simd_load_transpose;
	}

	int size = A.m * A.n;
	int bound = size - (size % 4);
	int i = 0;
	for (i; i < bound; i += 4) {
		__m128 colA = load_A(A, i);
		//printf("colA: %f, %f, %f, %f\n", A.matrix[ARRAY_INDEX(A, i)], A.matrix[ARRAY_INDEX(A, i + 1)], A.matrix[ARRAY_INDEX(A, i + 2)], A.matrix[ARRAY_INDEX(A, i + 3)]);

		__m128 colB = load_B(B, i);
		//printf("colB: %f, %f, %f, %f\n\n", B.matrix[ARRAY_INDEX(B, i)], B.matrix[ARRAY_INDEX(B, i + 1)], B.matrix[ARRAY_INDEX(B, i + 2)], B.matrix[ARRAY_INDEX(B, i + 3)]);
		//printf("next\n");
		__m128 colC = _mm_sub_ps(colA, colB);
		_mm_storeu_ps(&result.matrix[i], colC);	// store the contents of c into the current column location of C
	}

	// add any elements that don't fit into SIMD slots (up to 3)
	for (i; i < size; i++) {
		result.matrix[i] = A.matrix[ARRAY_INDEX(A, i)] - B.matrix[ARRAY_INDEX(B, i)];
	}

	return result;
}