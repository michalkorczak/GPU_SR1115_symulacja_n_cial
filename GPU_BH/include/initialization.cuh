#ifndef INITIALIZATION_CUH
#define INITIALIZATION_CUH

#include <iostream>
#include <math.h>

#include "BarnesHut.cuh"

#define INITIAL_SPREAD_X 10e3 // Initial spread of the bodies in x axis
#define INITIAL_SPREAD_Y 10e3 // Initial spread of the bodies in y axis
#define INITIAL_SPREAD_Z 10e3 // Initial spread of the bodies in z axis

typedef struct Config {
  int numberOfBodies = 10000;
  int iterations = 1000;
  int saveInterval = 100;
  float dt = 0.1;
  std::string outputFilename = "output.json";
} Config;

void initializeBodies(Bodies bodies, int numberOfBodies, double spreadX = INITIAL_SPREAD_X,
                        double spreadY = INITIAL_SPREAD_Y, double spreadZ = INITIAL_SPREAD_Z);
Config parseConfig(const int argc, const char** argv);
int getCudaBlockSize();

#endif