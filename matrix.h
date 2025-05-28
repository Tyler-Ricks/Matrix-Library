#ifndef MATRIX_H
#define MATRIX_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "memory_pool.h"

// include -DCMO in compilation to flag that you are in column major order
// default to row major order
// these macros handle matrix access, because fmatrices are store purely as arrays
#ifdef CMO
	#define MATRIX_AT(mat, i, j) ((mat.transpose) ? (mat.matrix[(i) * mat.n + (j)]) : (mat.matrix[(j) * mat.m + (i)]))
	#define INDEX_AT(mat, i, j) ((mat.transpose) ? ((i) * mat.n + (j)) : ((j) * mat.m + (i)))
	#define MAJOR(mat) (mat.n) // columns stored contiguously
	#define MINOR(mat) (mat.m) // rows stored in stride
#else
	#define MATRIX_AT(mat, i, j) ((mat.transpose) ? (mat.matrix[(j) * mat.m + (i)]) : (mat.matrix[(i) * mat.n + (j)]))
	#define INDEX_AT(mat, i, j) ((mat.transpose) ? ((j) * mat.m + (i)) : ((i) * mat.n + (j)))
	#define MAJOR(mat) (mat.m) // rows store contiguously
	#define MINOR(mat) (mat.n) // columns stored in stride
#endif 


// old implementation before transpose flag
#define OLD_MATRIX_AT(mat, i, j) (mat.matrix[i * mat.n + j])
#define OLD_INDEX_AT(mat, i, j) (i * mat.n + j) 

// error matrix
# define ERROR_FMATRIX (fmatrix){ 0, 0, NULL, 0 }
 /*
// I have a really strange idea that I want to work with later.
// I'll use the transpose operation as an example. normally, it takes O(mn), because we
// either copy the contents of the matrix in to another one, or rearrange elements from 
// the input matrix.
// However, I have an idea that could also make implementing column operations not as
// tedious.
// transpose could be done in constant time, by having a flag indicating a transpose in
// the struct. transpose in real life basically turns the matrix's rows into
// columns and columns into rows. In other words, we change how to read from the matrix
// from row major order into column major order. 
// This is how I want to make this work in constant time. All it would take to transpose
// a matrix is to change the method we use to read the matrix. (It would still take O(mn)
// time for not in-place algorithms so the matrix contents get copied into the new matrix)

// This also can generalize row operations and column operations into the same operation.
// A column operation can be done by just transposing a matrix, doing a row operation, 
// then transposing it back. transpose now takes constant time, so column operation 
// functions can be ridiculously simple.
// Of course, implementing it as a direct column operation is a little faster, but I 
// still like this trick

// To indicate that a matrix should be read in column major order, I'll include a flag in 
// the fmatrix struct. */

// float matrix
typedef struct{
	// rows, columns. Regardless of row or column major order, m is rows, n is columns
	int m, n;
	// pointer to item at [0,0]
	float* matrix; 
	// flag for transpose handling
	uint8_t transpose;
	// padding for cache
	uint8_t padding[3];
}fmatrix;

fmatrix create_fmatrix(int m, int n, float* matrix, pool *frame);
fmatrix fmatrix_create_identity(int m, int n, pool* frame);
fmatrix fmatrix_create_full(int m, int n, float x, pool* frame);

void print_fmatrix(fmatrix mat);
void print_fpool(pool *frame);

void print_properties(fmatrix mat);
void print_as_array(fmatrix mat);
//void print_memory_layout(fmatrix mat);
fmatrix fmatrix_copy_alloc(fmatrix mat, pool *frame);
fmatrix fmatrix_ncol_copy_alloc(fmatrix mat, int c, pool* frame);

void fswap(float *a, float *b);
void intswap(int *a, int *b);

fmatrix fmatrix_add_in(fmatrix matA, fmatrix matB);
fmatrix fmatrix_add(fmatrix matA, fmatrix matB, pool *frame);
fmatrix fmatrix_subtract_in(fmatrix matA, fmatrix matB);
fmatrix fmatrix_subtract(fmatrix matA, fmatrix matB, pool *frame);
fmatrix fmatrix_scale_in(fmatrix mat, float c);
fmatrix fmatrix_scale(fmatrix mat, float c, pool *frame);

float get_fmultiplied(fmatrix matA, fmatrix matB, int i, int j);
void fmatrix_multiply_in(fmatrix matA, fmatrix matB);
fmatrix fmatrix_multiply(fmatrix matA, fmatrix matB, pool *frame);

fmatrix fmatrix_transpose_in(fmatrix *mat);
fmatrix fmatrix_transpose(fmatrix mat, pool *frame);

fmatrix fmatrix_row_scale_in(fmatrix mat, int row, float c);
fmatrix fmatrix_row_scale(fmatrix mat, int row, float c, pool *frame);
fmatrix fmatrix_row_swap_in(fmatrix mat, int row1, int row2);
fmatrix fmatrix_row_swap(fmatrix mat, int row1, int row2, pool *frame);
fmatrix fmatrix_row_sum_in(fmatrix mat, int dest, float c1, int src, float c2);
fmatrix fmatrix_row_sum(fmatrix mat, int dest, float c1, int src, float c2, pool *frame);

fmatrix fmatrix_col_scale_in(fmatrix mat, int col, float c);
fmatrix fmatrix_col_scale(fmatrix mat, int col, float c, pool *frame);
fmatrix fmatrix_col_swap_in(fmatrix mat, int col1, int col2);
fmatrix fmatrix_col_swap(fmatrix mat, int col1, int col2, pool *frame);
fmatrix fmatrix_col_sum_in(fmatrix mat, int dest, float c1, int src, float c2);
fmatrix fmatrix_col_sum(fmatrix mat, int dest, float c1, int src, float c2, pool *frame);

int find_pivot_row(fmatrix mat, int pivot_row, int col);
float fmatrix_triangle_determinant(fmatrix mat, pool* frame);
float fmatrix_cofactor_expansion(fmatrix mat, int lr, int lc, int ur, int uc);
float fmatrix_determinant(fmatrix mat, pool *frame);

fmatrix fmatrix_inverse(fmatrix mat, pool* frame);

fmatrix fmatrix_col_space(fmatrix mat, pool* frame);
fmatrix fmatrix_row_space(fmatrix mat, pool* frame);

fmatrix* fmatrix_LU_factorize(fmatrix mat, fmatrix result[3], pool* frame);
fmatrix fmatrix_LU_solve(fmatrix A, fmatrix b, pool* frame);

// graphics stuff

#define PERSP_PROJ_DIMENSION 4
// I have a lot of macro plans for here due to the sheet variance across different graphics APIs and the 
// way they handle different matrices (ex perspective projection)
// openGL uses left hand rule and a different canonical view volume to Vulkan, which uses right hand rule.
// for now, implement for vulkan, but use macros for more flexibility

fmatrix ortho_proj_plane  (float l, float r, float b, float t, float n, float f, pool* frame);
fmatrix ortho_proj_plane_c(float r,			 float b,		   float n, float f, pool* frame);

fmatrix persp_proj_plane  (float l, float r, float b, float t, float n, float f, pool* frame);
fmatrix persp_proj_plane_c(float r,			 float b,		   float n, float f, pool* frame);
fmatrix persp_proj_fov	  (float w, float h, float theta,	   float n, float f, pool* frame);

#endif MATRIX_H
