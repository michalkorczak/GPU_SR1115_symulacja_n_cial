#include "../include/file_operations.cuh"
#include "../include/initialization.cuh"
#include "../include/structures.cuh"
#include <iostream>

#define BLOCK_SIZE 512
#define G 6.67430e-11 // Gravitational constant

void initializeBodies(Bodies& bodies, int numberOfBodies) { initializeRandomly(bodies, numberOfBodies); }

__global__ void computeBodyAcceleration(float3* position, float3* accelerations, float* masses, int n) {
  int index = blockDim.x * blockIdx.x + threadIdx.x;

  if (index >= n) {
    return;
  }

  const auto this_position = position[index];

  accelerations[index].x = 0;
  accelerations[index].y = 0;
  accelerations[index].z = 0;

  // this is just a prototype without any software optimization - 2 pair method
  for (int i = 0; i < n; i++) {
    if (i != index) {
      float3 distance;
      distance.x = position[i].x - this_position.x;
      distance.y = position[i].y - this_position.y;
      distance.z = position[i].z - this_position.z;

      float distanceSquared = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z + 0.01;
      float distanceCubed = distanceSquared * sqrt(distanceSquared);

      float acceleration = G * masses[i] / distanceCubed;

      accelerations[index].x += distance.x * acceleration;
      accelerations[index].y += distance.y * acceleration;
      accelerations[index].z += distance.z * acceleration;
    }
  }
}

__global__ void computeNewPositionAndSpeed(float3* positions, float3* velocities, float3* accelerations, float* masses,
                                           float dt, int n) {
  int index = blockDim.x * blockIdx.x + threadIdx.x;

  if (index >= n) {
    return;
  }

  velocities[index].x += accelerations[index].x * dt;
  velocities[index].y += accelerations[index].y * dt;
  velocities[index].z += accelerations[index].z * dt;

  positions[index].x += velocities[index].x * dt;
  positions[index].y += velocities[index].y * dt;
  positions[index].z += velocities[index].z * dt;
}

int main(const int argc, const char** argv) {
  srand(time(NULL));

  int numberOfBodies = 1000;
  if (argc > 1) {
    numberOfBodies = atoi(argv[1]);
  }

  float iterations = 1000;
  if (argc > 2) {
    iterations = atoi(argv[2]);
  }

  int saveInterval = 100;
  if (argc > 3) {
    saveInterval = atoi(argv[3]);
  }

  float dt = 0.01f;
  if (argc > 4) {
    dt = atof(argv[4]);
  }

  std::string outputFilename = "output.json";
  if (argc > 5) {
    outputFilename = argv[5];
  }

  int numberOfBlocks = (numberOfBodies + BLOCK_SIZE - 1) / BLOCK_SIZE;

  float* cpu_buffer = (float*)malloc(
      numberOfBodies * (sizeof(float) * 10)); // 10 floats per body - 1 mass + 3 position + 3 velocity + 3 acceleration
  Bodies cpu_bodies = {(float*)cpu_buffer, (float3*)(cpu_buffer + numberOfBodies),
                       (float3*)(cpu_buffer + 4 * numberOfBodies), (float3*)(cpu_buffer + 7 * numberOfBodies)};

  initializeBodies(cpu_bodies, numberOfBodies);

  float* gpu_buffer;
  cudaMalloc(&gpu_buffer, numberOfBodies * (sizeof(float) * 10));
  Bodies gpu_bodies = {(float*)gpu_buffer, (float3*)(gpu_buffer + numberOfBodies),
                       (float3*)(gpu_buffer + 4 * numberOfBodies), (float3*)(gpu_buffer + 7 * numberOfBodies)};

  for (int i = 0; i < iterations; i++) {
    cudaMemcpy(gpu_buffer, cpu_buffer, numberOfBodies * (sizeof(float) * 10), cudaMemcpyHostToDevice);

    computeBodyAcceleration<<<numberOfBlocks, BLOCK_SIZE>>>(gpu_bodies.position, gpu_bodies.acceleration,
                                                            gpu_bodies.mass, numberOfBodies);
    cudaDeviceSynchronize();

    computeNewPositionAndSpeed<<<numberOfBlocks, BLOCK_SIZE>>>(
        gpu_bodies.position, gpu_bodies.velocity, gpu_bodies.acceleration, gpu_bodies.mass, dt, numberOfBodies);
    cudaDeviceSynchronize();
    cudaMemcpy(cpu_buffer, gpu_buffer, numberOfBodies * (sizeof(float) * 10), cudaMemcpyDeviceToHost);

    if (saveInterval > 0 && i % saveInterval == 0) {
      writeFile(outputFilename, &cpu_bodies, i * dt, numberOfBodies, i > saveInterval);
    }
  }

  cudaFree(gpu_buffer);
}