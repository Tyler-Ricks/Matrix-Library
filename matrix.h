#ifndef MATRIX_H
#define MATRIX_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "memoryPool.h"

// macro so I can just use a 1D array for matrix struct
#define MATRIX_AT(mat, i, j) (mat.matrix[i, i * mat.n + j])
// macro to get array index given matrix index
#define INDEX_AT(mat, i, j) (i * mat.n + j) 
// error matrix
# define ERROR_FMATRIX (fmatrix){ 0, 0, NULL }
 /*
// I have a really strange idea that I want to work with later.
// I'll use the transpose operation as an example. Currently, it takes O(mn), because we
// either copy the contents of the matrix in to another one, or rearrange elements from 
// the input matrix.
// However, I have an idea that could also make implementing column operations not as
// tedious.
// If I somehow include in the fmatrix struct a way to read from the matrix, transpose 
// could be done in constant time. Transpose basically turns the matrix's rows into
// columns and columns into rows. In other words, we change how to read from the matrix
// from row major order into column major order. 
// This is how I want to make this work in
// constant time. All it would take to transpose a matrix is to change the method we use
// to read the matrix. (It would still take O(mn) time for not in-place algorithms so 
// the matrix contents get copied into the new matrix)

// This also can generalize row operations and column operations into the same operation.
// A column operation can be done by just transposing a matrix, doing a row operation, 
// then transposing it back. transpose now takes constant time, so column operation 
// functions can be ridiculously simple.
// Of course, implementing it as a direct column operation is a little faster, but I 
// still like this trick

// To indicate that a matrix should be read in column major order, m and n from the 
// fmatrix struct could be negative. This has issues because mat.m/mat.n are often
// used for loops and such. Also, I'm not too sure if a macro that handles the read
// conversion will be very simple at all.

// CHECKLIST:
//   1) Refactor the macros to support this idea 
//   2) Potentially refactor everything involving mat.m/mat.n to account for potentially
//      Negative values. 
//   3) There's some spots where I opted to not use the macros to remove redundancy. These
//      Will need to be fixed.
*/
// float matrix
typedef struct{
	// rows, columns
	int m, n;
	// pointer to item at [0,0]
	float* matrix; 
}fmatrix;

fmatrix create_fmatrix(int m, int n, float* matrix, pool *frame);
fmatrix fmatrix_copy_alloc(fmatrix mat, pool *frame);

void fswap(float *a, float *b);

fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame);
void fmatrix_add_in(fmatrix matA, fmatrix matB);
fmatrix fmatrix_subtract(fmatrix matA, fmatrix matB, pool *frame);
void fmatrix_subtract_in(fmatrix matA, fmatrix matB);
fmatrix fmatrix_scale(fmatrix mat, float c, pool *frame);
void fmatrix_scale_in(fmatrix mat, float c);

float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j);
fmatrix fmatrix_multiply(fmatrix matA, fmatrix matB, pool *frame);
void fmatrix_multiply_in(fmatrix matA, fmatrix matB);

fmatrix fmatrix_transpose(fmatrix mat, pool *frame);
void fmatrix_transpose_in(fmatrix mat);

fmatrix fmatrix_row_scale(fmatrix mat, int row, float c, pool *frame);
void fmatrix_row_scale_in(fmatrix mat, int row, float c);
fmatrix fmatrix_row_swap(fmatrix mat, int row1, int row2, pool *frame);
void fmatrix_row_swap_in(fmatrix mat, int row1, int row2);
fmatrix fmatrix_row_sum(fmatrix mat, int dest, float c1, int src, float c2, pool *frame);
void fmatrix_row_sum_in(fmatrix mat, int dest, float c1, int src, float c2);




void print_fmatrix(fmatrix mat);

#endif MATRIX_H
