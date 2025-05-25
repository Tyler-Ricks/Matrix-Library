#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>

float dot(fmatrix u, fmatrix v);

fmatrix cross(fmatrix u, fmatrix v, pool* frame);

float magnitude(fmatrix u);

float distance(fmatrix u, fmatrix v, pool* frame);

void normalize_in(fmatrix u);
fmatrix normalize(fmatrix u, pool* frame);

float angle(fmatrix u, fmatrix v);

fmatrix proj(fmatrix a, fmatrix b, pool* frame);
fmatrix proj_n(fmatrix a, fmatrix b, pool* frame);

fmatrix reflect(fmatrix u, fmatrix v, pool* frame);
fmatrix reflect_n(fmatrix u, fmatrix v, pool* frame);

#endif