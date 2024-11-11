#ifndef INITIALIZATION_CUH
#define INITIALIZATION_CUH

#include <math.h>

#include "structures.cuh"

#define INITIAL_SPREAD_WIDTH 10e3  // Initial spread of the bodies in x axis
#define INITIAL_SPREAD_HEIGHT 10e3 // Initial spread of the bodies in y axis

void initializeRandomly(Bodies& bodies, int numberOfBodies, float spreadWidth = INITIAL_SPREAD_WIDTH,
                        float spreadHeight = INITIAL_SPREAD_HEIGHT);

#endif