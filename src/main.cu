#include <iostream>
#include "../include/structures.cuh"
#include "../include/initialization.cuh"

#define BLOCK_SIZE 256
#define G 6.67430e-11            // Gravitational constant

void initializeBodies(Bodies& bodies, int numberOfBodies) {
  initializeRandomly(bodies, numberOfBodies);
}

__global__ void computeBodyAcceleration(float2* position, float2* accelerations, float* masses, int n) {
  int index = blockDim.x * blockIdx.x + threadIdx.x;

  if (index >= n) {
    return;
  }

  const auto this_position = position[index];

  accelerations[index].x = 0;
  accelerations[index].y = 0;

  // this is just a prototype without any software optimization - 2 pair method
  for (int i = 0; i < n; i++) {
    if (i != index) {
      float2 distance;
      distance.x = position[i].x - this_position.x;
      distance.y = position[i].y - this_position.y;

      float distanceSquared = distance.x * distance.x + distance.y * distance.y + 0.01;
      float distanceCubed = distanceSquared * sqrt(distanceSquared);

      float acceleration = G * masses[i] / distanceCubed;

      accelerations[index].x += distance.x * acceleration;
      accelerations[index].y += distance.y * acceleration;
    }
  }
}

__global__ void computeNewPositionAndSpeed(float2* positions, float2* velocities, float2* accelerations, float* masses,
                                           float dt, int n) {
  int index = blockDim.x * blockIdx.x + threadIdx.x;

  if (index >= n) {
    return;
  }

  velocities[index].x += accelerations[index].x * dt;
  velocities[index].y += accelerations[index].y * dt;

  positions[index].x += velocities[index].x * dt;
  positions[index].y += velocities[index].y * dt;
}

int main(const int argc, const char** argv) {
  srand(time(NULL));

  int numberOfBodies = 1000; // default value
  if (argc > 1) {
    numberOfBodies = atoi(argv[1]);
  }

  float iterations = 1000;
  if (argc > 2) {
    iterations = atoi(argv[2]);
  }

  int printInterval = 100;
  if (argc > 3) {
    printInterval = atoi(argv[3]);
  }

  float dt = 0.01f;
  if (argc > 4) {
    dt = atof(argv[4]);
  }

  int numberOfBlocks = (numberOfBodies + BLOCK_SIZE - 1) / BLOCK_SIZE;

  float* cpu_buffer = (float*)malloc(
      numberOfBodies * (sizeof(float) * 7)); // 7 floats per body - 1 mass + 2 position + 2 velocity + 2 acceleration
  Bodies cpu_bodies = {(float*)cpu_buffer, (float2*)(cpu_buffer + numberOfBodies),
                   (float2*)(cpu_buffer + 3 * numberOfBodies), (float2*)(cpu_buffer + 5 * numberOfBodies)};

  initializeBodies(cpu_bodies, numberOfBodies);

  float* gpu_buffer;
  cudaMalloc(&gpu_buffer, numberOfBodies * (sizeof(float) * 7));
  Bodies gpu_bodies = {(float*)gpu_buffer, (float2*)(gpu_buffer + numberOfBodies),
                   (float2*)(gpu_buffer + 3 * numberOfBodies), (float2*)(gpu_buffer + 5 * numberOfBodies)};

  for (int i = 0; i < iterations; i++) {
    cudaMemcpy(gpu_buffer, cpu_buffer, numberOfBodies * (sizeof(float) * 7), cudaMemcpyHostToDevice);

    computeBodyAcceleration<<<numberOfBlocks, BLOCK_SIZE>>>(gpu_bodies.position, gpu_bodies.acceleration, gpu_bodies.mass,
                                                            numberOfBodies);
    cudaDeviceSynchronize();

    computeNewPositionAndSpeed<<<numberOfBlocks, BLOCK_SIZE>>>(gpu_bodies.position, gpu_bodies.velocity, gpu_bodies.acceleration,
                                                               gpu_bodies.mass, dt, numberOfBodies);
    cudaDeviceSynchronize();
    cudaMemcpy(cpu_buffer, gpu_buffer, numberOfBodies * (sizeof(float) * 7), cudaMemcpyDeviceToHost);


    if (printInterval > 0 && i % printInterval == 0)
    {
      std::cout << "ITERATION: " << i + 1 << "----------------" << std::endl;
      for (int j = 0; j < numberOfBodies; j++) {
        std::cout << "Number: " << j << " - x: " << cpu_bodies.position[j].x << " - y: " << cpu_bodies.position[j].y << std::endl;
      }
    }
  }

  cudaFree(gpu_buffer);
}