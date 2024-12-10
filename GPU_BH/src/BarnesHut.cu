#include "../include/BarnesHut.cuh"

// calculate maximum of doubles atomically
__device__ void atomicMaxDouble(double* address, double val) {
  unsigned long long* address_as_ull = reinterpret_cast<unsigned long long int*>(address);
  unsigned long long old = *address_as_ull, assumed;
  unsigned long long new_val = *reinterpret_cast<unsigned long long*>(&val);

  while (old < new_val) {
    old = atomicCAS(address_as_ull, assumed, new_val);
  }
}

__device__ void atomicMinDouble(double* address, double val) {
  unsigned long long* address_as_ull = reinterpret_cast<unsigned long long int*>(address);
  unsigned long long old = *address_as_ull, assumed;
  unsigned long long new_val = *reinterpret_cast<unsigned long long*>(&val);

  while (old > new_val) {
    old = atomicCAS(address_as_ull, assumed, new_val);
  }
}

__global__ void computeBoundingBox(const Bodies& bodies, int n, double* bounds) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= n) {
    return;
  }

  extern __shared__ double shared_data[];

  double* x_min = shared_data;
  double* x_max = shared_data + blockDim.x;
  double* y_min = shared_data + 2 * blockDim.x;
  double* y_max = shared_data + 3 * blockDim.x;
  double* z_min = shared_data + 4 * blockDim.x;
  double* z_max = shared_data + 5 * blockDim.x;

  if (idx < n) {
    x_min[threadIdx.x] = bodies.position[idx].x;
    x_max[threadIdx.x] = bodies.position[idx].x;
    y_min[threadIdx.x] = bodies.position[idx].y;
    y_max[threadIdx.x] = bodies.position[idx].y;
    z_min[threadIdx.x] = bodies.position[idx].z;
    z_max[threadIdx.x] = bodies.position[idx].z;
  } else {
    // set to extreme values to not interfere with the reduction
    x_min[threadIdx.x] = DBL_MAX;
    x_max[threadIdx.x] = -DBL_MAX;
    y_min[threadIdx.x] = DBL_MAX;
    y_max[threadIdx.x] = -DBL_MAX;
    z_min[threadIdx.x] = DBL_MAX;
    z_max[threadIdx.x] = -DBL_MAX;
  }

  __syncthreads();

  // reduction
  // basically a tournament tree for comparing the values
  for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
    if (threadIdx.x < stride) {
      x_min[threadIdx.x] = fmin(x_min[threadIdx.x], x_min[threadIdx.x + stride]);
      x_max[threadIdx.x] = fmax(x_max[threadIdx.x], x_max[threadIdx.x + stride]);
      y_min[threadIdx.x] = fmin(y_min[threadIdx.x], y_min[threadIdx.x + stride]);
      y_max[threadIdx.x] = fmax(y_max[threadIdx.x], y_max[threadIdx.x + stride]);
      z_min[threadIdx.x] = fmin(z_min[threadIdx.x], z_min[threadIdx.x + stride]);
      z_max[threadIdx.x] = fmax(z_max[threadIdx.x], z_max[threadIdx.x + stride]);
    }
    __syncthreads();
  }

  if (threadIdx.x == 0) {
    atomicMinDouble(&bounds[0], x_min[0]);
    atomicMaxDouble(&bounds[1], x_max[0]);
    atomicMinDouble(&bounds[2], y_min[0]);
    atomicMaxDouble(&bounds[3], y_max[0]);
  }
}

__device__ void insertBody(TreeNode* node, const Bodies& bodies, int bodyIdx, double x_min, double x_max, double y_min,
                           double y_max, double z_min, double z_max) {
  double x = bodies.position[bodyIdx].x;
  double y = bodies.position[bodyIdx].y;
  double z = bodies.position[bodyIdx].z;

  if (node->isLeaf) {
    if (node->bodyIndex == -1) {
      node->bodyIndex = bodyIdx;
    } else {
      double x_mid = (node->x_min + node->x_max) / 2;
      double y_mid = (node->y_min + node->y_max) / 2;
      double z_mid = (node->z_min + node->z_max) / 2;

      bool x_lower = x < x_mid;
      bool y_lower = y < y_mid;
      bool z_lower = z < z_mid;

      int childIndex = (x_lower ? 0 : 1) + (y_lower ? 0 : 2) + (z_lower ? 0 : 4);

      insertBody(node->children[childIndex], bodies, bodyIdx, x_min, x_max, y_min, y_max, z_min, z_max);
    }
  }
}

__global__ void buildTree(TreeNode* nodes, Bodies& bodies, int n, double x_min, double x_max, double y_min,
                          double y_max, double z_min, double z_max) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= n) {
    return;
  }

  TreeNode* node = &nodes[0];

  insertBody(node, bodies, idx, x_min, x_max, y_min, y_max, z_min, z_max);
}

__global__ void computeCentersOfMass(TreeNode* nodes, const Bodies& bodies, int n) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= n) {
    return;
  }

  TreeNode* node = &nodes[idx];

  if (node->isLeaf) {
    // for leaf nodes, the center of mass is the body itself
    int bodyIdx = node->bodyIndex;
    node->cx = bodies.position[bodyIdx].x;
    node->cy = bodies.position[bodyIdx].y;
    node->cz = bodies.position[bodyIdx].z;
    node->mass = bodies.mass[bodyIdx];
  } else {
    // for internal nodes, the center of mass is the weighted average of the children's center of mass
    double totalMass = 0.0;
    double cx = 0.0, cy = 0.0, cz = 0.0;

    // calculate center of mass
    for (int i = 0; i < 8; i++) {
      TreeNode* child = node->children[i];
      if (node->children[i] != nullptr) {
        totalMass += node->children[i]->mass;
        cx += child->cx * child->mass;
        cy += child->cy * child->mass;
        cz += child->cz * child->mass;
      }
    }

    if (totalMass > 0.0) {
      node->cx = cx / totalMass;
      node->cy = cy / totalMass;
      node->cz = cz / totalMass;
    }
    node->mass = totalMass;
  }
}

__device__ void computeForcesRecursively(TreeNode* node, const Bodies& bodies, int idx, double mass, double x, double y,
                                         double z, double* fx, double* fy, double* fz, double theta) {
  if (node == nullptr) {
    return;
  }

  // calculate distance between the body and the center of mass of the node
  double dx = node->cx - x;
  double dy = node->cy - y;
  double dz = node->cz - z;
  double distance = sqrt(dx * dx + dy * dy + dz * dz);

  if (node->isLeaf) {
    if (node->bodyIndex != idx) {
      // calculate force between the body and the node
      double distanceSquared = distance * distance;
      double distanceCubed = distanceSquared * distance;
      double acceleration = G * node->mass / distanceCubed;

      *fx += dx * acceleration;
      *fy += dy * acceleration;
      *fz += dz * acceleration;
    }
  } else {
    if (distance / (node->x_max - node->x_min) < theta) {
      // if the ratio of the distance to the size of the node is less than theta, use the center of mass as a single
      // body
      double distanceSquared = distance * distance;
      double distanceCubed = distanceSquared * distance;
      double acceleration = G * node->mass / distanceCubed;

      *fx += dx * acceleration;
      *fy += dy * acceleration;
      *fz += dz * acceleration;
    } else {
      // otherwise, recursively calculate the forces from the children
      for (int i = 0; i < 8; i++) {
        computeForcesRecursively(node->children[i], bodies, idx, mass, x, y, z, fx, fy, fz, theta);
      }
    }
  }
}

__global__ void computeForces(TreeNode* nodes, const Bodies& bodies, double* forces_x, double* forces_y,
                              double* forces_z, int n, double theta) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= n) {
    return;
  }

  double fx = 0.0, fy = 0.0, fz = 0.0;
  double x = bodies.position[idx].x;
  double y = bodies.position[idx].y;
  double z = bodies.position[idx].z;
  double mass = bodies.mass[idx];

  // transverse the tree to calculate the forces
  TreeNode* node = &nodes[0];
  computeForcesRecursively(node, bodies, idx, mass, x, y, z, &fx, &fy, &fz, theta);
}

__global__ void updateBodies(Bodies bodies, int n, double* fx, double* fy, double* fz, double dt) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= n) {
    return;
  }

  const double mass = bodies.mass[idx];

  bodies.acceleration[idx].x = fx[idx] / mass;
  bodies.acceleration[idx].y = fy[idx] / mass;
  bodies.acceleration[idx].z = fz[idx] / mass;

  bodies.velocity[idx].x += bodies.acceleration[idx].x * dt;
  bodies.velocity[idx].y += bodies.acceleration[idx].y * dt;
  bodies.velocity[idx].z += bodies.acceleration[idx].z * dt;

  bodies.position[idx].x += bodies.velocity[idx].x * dt;
  bodies.position[idx].y += bodies.velocity[idx].y * dt;
  bodies.position[idx].z += bodies.velocity[idx].z * dt;
}
