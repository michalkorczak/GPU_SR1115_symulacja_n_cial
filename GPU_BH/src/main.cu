#include "../include/BarnesHut.cuh"
#include "../include/file_operations.cuh"
#include "../include/initialization.cuh"
#include <iostream>

// #define BLOCK_SIZE 256
#define G 6.67430e-11 // Gravitational constant

void cleanup(Bodies& bodies, TreeNode* nodes) {
  cudaFree(bodies.mass);
  cudaFree(bodies.position);
  cudaFree(bodies.velocity);
  cudaFree(bodies.acceleration);

  cudaFree(nodes);
}

int main(const int argc, const char** argv) {
  srand(time(NULL));
  Config config = parseConfig(argc, argv);

  int blockSize = getCudaBlockSize();

  // GPU bodies
  double* gpu_buffer;
  cudaMalloc(&gpu_buffer, config.numberOfBodies * (sizeof(double) * 10));
  Bodies gpu_bodies = {(double*)gpu_buffer, (double3*)(gpu_buffer + config.numberOfBodies),
                       (double3*)(gpu_buffer + 3 * config.numberOfBodies), (double3*)(gpu_buffer + 5 * config.numberOfBodies)};

  // CPU bodies
  double* cpu_buffer = (double*)malloc(config.numberOfBodies * sizeof(double) * 10);
  Bodies cpu_bodies = {(double*)cpu_buffer, (double3*)(cpu_buffer + config.numberOfBodies),
                       (double3*)(cpu_buffer + 4 * config.numberOfBodies),
                       (double3*)(cpu_buffer + 7 * config.numberOfBodies)};

  initializeBodies(cpu_bodies, config.numberOfBodies);


  cudaMemcpy(gpu_buffer, cpu_buffer, config.numberOfBodies * (sizeof(double) * 10), cudaMemcpyHostToDevice);

  int maxNodes = 8 * config.numberOfBodies;
  TreeNode* nodes;
  cudaMalloc((void**)&nodes, sizeof(TreeNode) * maxNodes);

  TreeNode* nodesCPU = (TreeNode*)malloc(config.numberOfBodies * sizeof(TreeNode) * maxNodes);

  double cpuBounds[6] = {0};

  double bounds[6];
  cudaMalloc((void**)bounds, sizeof(double));

  cudaMemcpy(bounds, cpuBounds, 6 * sizeof(double), cudaMemcpyHostToDevice);

  int numberOfBlocks = (config.numberOfBodies + blockSize - 1) / blockSize;

  double *fx, *fy, *fz;
  cudaMalloc((void**)&fx, sizeof(double) * config.numberOfBodies);
  cudaMalloc((void**)&fy, sizeof(double) * config.numberOfBodies);
  cudaMalloc((void**)&fz, sizeof(double) * config.numberOfBodies);

  for (int i = 0; i < config.iterations; i++) {
    computeBoundingBox<<<numberOfBlocks, blockSize>>>(gpu_bodies, config.numberOfBodies, bounds);
    cudaDeviceSynchronize();
    buildTree<<<numberOfBlocks, blockSize>>>(nodes, gpu_bodies, config.numberOfBodies, bounds[0], bounds[1], bounds[2],
                                             bounds[3], bounds[4], bounds[5]);
    cudaDeviceSynchronize();
    computeForces<<<numberOfBlocks, blockSize>>>(nodes, gpu_bodies, fx, fy, fz, config.numberOfBodies, 10);
    cudaDeviceSynchronize();
    updateBodies<<<numberOfBlocks, blockSize>>>(gpu_bodies, config.numberOfBodies, fx, fy, fz, config.dt);

    if (config.saveInterval > 0 && i % config.saveInterval == 0) {
      cudaDeviceSynchronize();
      cudaMemcpy(cpu_bodies.mass, gpu_bodies.mass, config.numberOfBodies * sizeof(double), cudaMemcpyDeviceToHost);
      cudaMemcpy(cpu_bodies.acceleration, gpu_bodies.acceleration, config.numberOfBodies * (sizeof(double) * 3),
                 cudaMemcpyDeviceToHost);
      cudaMemcpy(cpu_bodies.position, gpu_bodies.position, config.numberOfBodies * (sizeof(double) * 3),
                 cudaMemcpyDeviceToHost);
      cudaMemcpy(cpu_bodies.velocity, gpu_bodies.velocity, config.numberOfBodies * (sizeof(double) * 3),
                 cudaMemcpyDeviceToHost);
      writeFile(config.outputFilename, &cpu_bodies, i * config.dt, config.numberOfBodies, i > config.saveInterval);
    }
  }

  cleanup(gpu_bodies, nodes);
  cudaFree(fx);
  cudaFree(fy);
  cudaFree(fz);
  cudaFree(bounds);
  free(cpu_buffer);
}