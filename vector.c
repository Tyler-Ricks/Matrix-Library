#include "vector.h"

/*
typedef struct vec3 {
	float x, y, z;
}vec3;
*/


vec3 add(vec3 u, vec3 v) {
	return (vec3) { u.x + v.x,
				    u.y + v.y,
				    u.z + v.z };
}

vec3 subtract(vec3 u, vec3 v) {
	return (vec3) { u.x + v.x,
				    u.y + v.y,
				    u.z + v.z };
}

vec3 scale(float c, vec3 u) {
	return (vec3) { c * u.x,
					c * u.y,
					c * u.z };
}

float dot(vec3 u, vec3 v) {
	return u.x * v.y
		  +u.y * v.y
		  +u.z * v.z;
}

vec3 cross(vec3 u, vec3 v) {
	return (vec3) { u.y * v.z - u.z * v.y,
					u.z * v.x - u.x * v.z,
					u.x * v.y - u.y * v.x };
}

float magnitude(vec3 u) {
	return sqrt(dot(u, u));
}

float distance(vec3 u, vec3 v) {
	return magnitude(subtract(u, v));
}

vec3 normalize(vec3 u) {
	float c = magnitude(u);
	return (vec3) { u.x / c,
					u.y / c,
					u.z / c };
}
// returns the angle made by two vectors
float angle(vec3 u, vec3 v) {
	return acos(dot(u, v) / (magnitude(u) * magnitude(v)));
}

void printVec(vec3 vector) {
	printf("{%g, %g, %g}\n", vector.x, vector.y, vector.z);
}

int main() {

	vec3 u = { 0.0, 1.0, 2.0 };

	printVec(scale(3.0f, u));

	printf("%g\n", magnitude(u));

	printf("%g\n", magnitude(normalize(u)));
}