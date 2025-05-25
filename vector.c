#include "vector.h"

/*
typedef struct fmatrix {
	float x, y, z;
}fmatrix;
*/

float dot(fmatrix u, fmatrix v) {
	if (u.n != 1 || v.n != 1) {
		printf("input is not a vector\n");
		exit(0);
	}
	if (u.m != v.m) {
		printf("vector size mismatch!\n");
		exit(0);
	}

	float result = 0.0f;
	for (int i = 0; i < u.m; i++) {
		result += u.matrix[i] * v.matrix[i];
	}
}

fmatrix cross(fmatrix u, fmatrix v, pool* frame) {
	if (u.m != 3 || v.m != 3) {
		printf("input vectors must be size 3\n");
		return ERROR_FMATRIX;
	}

	float *a = u.matrix;
	float *b = v.matrix;
	float result[] = {	a[1] * b[2] - a[2] * b[1],
						a[2] * b[0] - a[0] * b[3],
						a[0] * b[1] - a[1] * b[0]};

	return create_fmatrix(u.m, 1, result, frame);
}

float magnitude(fmatrix u) {
	return (float) sqrtf(dot(u, u));
}

float distance(fmatrix u, fmatrix v, pool* frame) {
	fmatrix sub = fmatrix_subtract(u, v, frame);
	float dist = magnitude(sub);

	pool_free_from(sub.matrix, frame);

	return dist;
}

fmatrix normalize_in(fmatrix u) {
	if (u.n != 1) {
		printf("you may only normalize vectors!\n");
		return ERROR_FMATRIX;
	}
	float m = magnitude(u);
	for (int i = 0; i < u.m; i++) {
		u.matrix[i] /= m;
	}

	return u;
}

fmatrix normalize(fmatrix u, pool* frame) {
	fmatrix result = fmatrix_copy_alloc(u, frame);
	normalize_in(result);
	return result;
}
// returns the angle made by two vectors
// theoretically you could allow dimension mismatch? Just replace non-existing elements with 0
// Maybe I'll make a seperate function for that
float angle(fmatrix u, fmatrix v) {
	if (u.n != 1 || v.n != 1) {
		printf("You may only find the angle between vectors\n");
		exit(0);
	}
	if (u.m != v.m) {
		printf("Vector dimension mismatch when finding angle\n");
		exit(0);
	}

	return acosf(dot(u, v) / (magnitude(u) * magnitude(v)));
}

// returns the projection of vector a onto b, storing the vector on the frame
fmatrix proj(fmatrix a, fmatrix b, pool* frame) {
	if(a.n != 1 || b.n != 1){
		printf("You may only find projections of vectors onto vectors\m");
		return ERROR_FMATRIX;
	}
	if (a.m != b.m) {
		printf("You may only find projections between vectors of the same dimension\n");
		return ERROR_FMATRIX;
	}
	float c = dot(a, b) / dot(b, b);
	return fmatrix_scale(b, c, frame);
}

// returns the projection of vector a onto b, storing the vector on the frame, assuming that b is a normal vector
fmatrix proj_n(fmatrix a, fmatrix b, pool* frame) {
	if(a.n != 1 || b.n != 1){
		printf("You may only find projections of vectors onto vectors\m");
		return ERROR_FMATRIX;
	}
	if (a.m != b.m) {
		printf("You may only find projections between vectors of the same dimension\n");
		return ERROR_FMATRIX;
	}
	float c = dot(a, b);
	return fmatrix_scale(b, c, frame);
}

// returns the reflection of u across v
fmatrix reflect(fmatrix a, fmatrix b, pool* frame) {
	if(a.n != 1 || b.n != 1){
		printf("You may only find projections of vectors onto vectors\m");
		return ERROR_FMATRIX;
	}
	if (a.m != b.m) {
		printf("You may only find projections between vectors of the same dimension\n");
		return ERROR_FMATRIX;
	}

	// the reflection of a across vector b
	// r = -2proj(a, b) + a
	fmatrix r = proj(a, b, frame);

	return fmatrix_add_in(fmatrix_scale_in(r, -2.0f), b);
}

// returns the reflection of a across b, assuming b is a normal vector
fmatrix reflect_n(fmatrix a, fmatrix b, pool* frame) {
	if(a.n != 1 || b.n != 1){
		printf("You may only find projections of vectors onto vectors\m");
		return ERROR_FMATRIX;
	}
	if (a.m != b.m) {
		printf("You may only find projections between vectors of the same dimension\n");
		return ERROR_FMATRIX;
	}

	// the reflection of a across vector b
	// r = -2proj(a, b) + a
	fmatrix r = proj_n(a, b, frame);

	return fmatrix_add_in(fmatrix_scale_in(r, -2.0f), b);
}