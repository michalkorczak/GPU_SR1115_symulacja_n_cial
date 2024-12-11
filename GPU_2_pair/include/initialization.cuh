#ifndef INITIALIZATION_CUH
#define INITIALIZATION_CUH

#include <math.h>

#include "structures.cuh"

#define INITIAL_SPREAD_X 10e3  // Initial spread of the bodies in x axis
#define INITIAL_SPREAD_Y 10e3 // Initial spread of the bodies in y axis
#define INITIAL_SPREAD_Z 10e3 // Initial spread of the bodies in z axis

void initializeRandomly(Bodies& bodies, int numberOfBodies, float spreadX = INITIAL_SPREAD_X,
                        float spreadY = INITIAL_SPREAD_Y, float spreadZ = INITIAL_SPREAD_Z);

#endif