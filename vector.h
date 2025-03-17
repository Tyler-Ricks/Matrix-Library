#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct vec3{
	float x, y, z;
}vec3;

vec3 add(vec3 u, vec3 v);
vec3 subtract(vec3 u, vec3 v);
vec3 scale(float c, vec3 v);

float dot(vec3 u, vec3 v);

vec3 cross(vec3 u, vec3 v);

float magnitude(vec3 u);

float distance(vec3 u, vec3 v);

vec3 normalize(vec3 u);

float angle(vec3 u, vec3 v);

void printVec(vec3 v);