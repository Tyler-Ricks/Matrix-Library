#include <stdio.h>
#include <math.h>

typedef struct{
	// rows, columns
	int r, c;
	// pointer to item at [0,0]
	float* place; 
}matrix;
