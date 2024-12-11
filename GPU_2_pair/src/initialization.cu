#include "../include/initialization.cuh"

void initializeRandomly(Bodies& bodies, int numberOfBodies, float spreadX, float spreadY, float spreadZ) {
  float mass = 10e15;

  for (int i = 0; i < numberOfBodies; i++) {
    // initialize the bodies randomly spread around 0,0
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
    float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
    float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
    bodies.position[i].x = x * spreadX;
    bodies.position[i].y = y * spreadY;
    bodies.position[i].z = z * spreadZ;


    bodies.velocity[i].x = 0;
    bodies.velocity[i].y = 0;
    bodies.velocity[i].z = 0;

    bodies.mass[i] = mass;

    bodies.acceleration[i].x = 0;
    bodies.acceleration[i].y = 0;
    bodies.acceleration[i].z = 0;
  }
}