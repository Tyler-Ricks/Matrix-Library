#include "graphics_tools.h"


// returns a perspective projection matrix without assuming that the given volume parameters are centered on the z-axis
fmatrix persp_proj_plane(float l, float r, float b, float t, float n, float f, pool* frame) {
	float P[16] = { (2.0f * n) / (r - l),  0.0f,				    0.0f,		   0.0f,
					 0.0f,				  (2.0f * n) / (b - t),     0.0f,		   0.0f,
					 0.0f,				   0.0f,					f / (f - n),  (-f * n) / (f - n),
					 0.0f,				   0.0f,					1.0f,		   0.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}

// returns a perspective projection matrix, assuming that the given volume parameters are centered on the z-axis
// If we are centered around the z-axis, we can say a few things: 
//		r = -l -> r + l = 0, and r - l = 2r
//		b = -t -> b + t = 0, and b - t = 2b
// These allow a few simplifications in the matrix at the cost of being more careful with how we implement other things.
fmatrix persp_proj_plane_c(float r, float b, float n, float f, pool* frame) {
	float P[16] = {  n/r,			0.0f,		0.0f,			0.0f,
				 	 0.0f,			n/b,		0.0f,		    0.0f,
					 0.0f,			0.0f,		f / (f - n),   (-f * n) / (f - n),
					 0.0f,			0.0f,		1.0f,		    0.0f};

	return create_fmatrix(PERSP_PROJ_DIMENSION, PERSP_PROJ_DIMENSION, P, frame);
}