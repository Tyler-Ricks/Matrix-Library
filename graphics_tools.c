#include "matrix.h"

// this file contains graphics-specific utilities	

// Projection matrices:
// These matrices ultimately transform from some view volume to a given canonical view volume. 
// Most are composed of multiple matrices. The breakdown of all this can be found online, but 
// I'll explain stuff that isn't as easily found.

// returns an orthographics projection matrix given the coordinates of an orthographic view volume
fmatrix ortho_proj_plane(float l, float r, float b, float t, float n, float f, pool* frame) {

	// row major
	/*float P[16] = {2.0f / (r - l),	0.0f,				0.0f,				(r + l) / (r - l),
					0.0f,				2.0f / (b - t),		0.0f,				(b + t) / (b - t),
					0.0f,				0.0f,				1.0f / (f - n),		-n / (f - n),
					0.0f,				0.0f,				0.0f,				 1.0f};*/

	// column major
	float P[16] = { 2.0f / (r - l),		0.0f,				0.0f,				 0.0f,
					0.0f,				2.0f / (b - t),		0.0f,				 0.0f,
					0.0f,				0.0f,				1.0f / (f - n),		 0.0f,
					(r + l) / (r - l),	(b + t) / (b - t),	-n / (f - n),		 1.0f};


	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}

// returns an orthographics projection matrix given the coordinates of an orthographic view volume, 
// assuming that the OVV was centered on the z-axis
//		r = -l -> r + l = 0, and r - l = 2r
//		b = -t -> b + t = 0, and b - t = 2b
fmatrix ortho_proj_plane_c(float r, float b, float n, float f, pool* frame) {

	// row major 
	/*float P[16] = {1.0f / r,		0.0f,			0.0f,				0.0f,
					0.0f,			1.0f / b,		0.0f,				0.0f,
					0.0f,			0.0f,			1.0f / (f - n),		-n / (f - n),
					0.0f,			0.0f,			0.0f,				1.0f};*/

	// column major
	float P[16] = { 1.0f / r,		0.0f,			0.0f,				0.0f,
					0.0f,			1.0f / b,		0.0f,				0.0f,
					0.0f,			0.0f,			1.0f / (f - n),		0.0f,
					0.0f,			0.0f,			-n / (f - n),		1.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}

// returns a perspective projection matrix without assuming that the given volume parameters are centered on the z-axis
fmatrix persp_proj_plane(float l, float r, float b, float t, float n, float f, pool* frame) {

	// row major
	/*float P[16] = {(2.0f * n) / (r - l),  0.0f,				    0.0f,				0.0f,
					 0.0f,				  (2.0f * n) / (b - t),     0.0f,				0.0f,
					 0.0f,				   0.0f,					f / (f - n),		(-f * n) / (f - n),
					 0.0f,				   0.0f,					1.0f,				0.0f};*/

	// column major
	float P[16] = { (2.0f * n) / (r - l),  0.0f,				    0.0f,				0.0f,
					0.0f,				  (2.0f * n) / (b - t),     0.0f,				0.0f,
					0.0f,				   0.0f,					f / (f - n),		1.0f,
					0.0f,				   0.0f,					(-f * n) / (f - n),	0.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}

// returns a perspective projection matrix, assuming that the given volume parameters are centered on the z-axis
// If we are centered around the z-axis, we can say a few things: 
//		r = -l -> r + l = 0, and r - l = 2r
//		b = -t -> b + t = 0, and b - t = 2b
// These allow a few simplifications in the matrix at the cost of being more careful with how we implement other things.
fmatrix persp_proj_plane_c(float r, float b, float n, float f, pool* frame) {

	// row major
	/*float P[16] = {n / r,			0.0f,		0.0f,					0.0f,
				 	 0.0f,			n/b,		0.0f,					0.0f,
					 0.0f,			0.0f,		f / (f - n),			(-f * n) / (f - n),
					 0.0f,			0.0f,		1.0f,					0.0f};*/

	// column major
	float P[16] = {  n/r,			0.0f,		0.0f,					0.0f,
					 0.0f,			n/b,		0.0f,					0.0f,
					 0.0f,			0.0f,		f / (f - n),			1.0f,
					 0.0f,			0.0f,		(-f * n) / (f - n),		0.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}

// returns a perspective projection matrix given aspect ration (w/h), near and far planes, and vertical fov theta
fmatrix persp_proj_fov(float w, float h, float n, float f, float theta, pool* frame) {
	float a_r = w / h;			// aspect ratio
	float a = theta / 2.0f;		

	// row major order
	/*float P[16] = {1.0f / (a_r * tan(a)),	0.0f,				0.0f,					0.0f,
					0.0f,					1.0f / tan(a),		0.0f,					0.0f,
					0.0f,					0.0f,				f / (f - n),			(-f * n) / (f - n),
					0.0f,					0.0f,				1.0f,					0.0f};*/

	// column major order
	float P[16] = { 1.0f / (a_r * tan(a)),	0.0f,				0.0f,					0.0f,
					0.0f,					1.0f / tan(a),		0.0f,					0.0f,
					0.0f,					0.0f,				f / (f - n),			1.0f,
					0.0f,					0.0f,				(-f * n) / (f - n),		0.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}
