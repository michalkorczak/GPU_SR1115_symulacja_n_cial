#ifndef BARNESHUT_CUH
#define BARNESHUT_CUH

#include <cfloat>
#define G 6.67430e-11

// Structure Of Arrays (SoA) for bodies
typedef struct {
  double* mass;
  double3* position;
  double3* velocity;
  double3* acceleration;
} Bodies;

struct TreeNode {
  double x_min, x_max, y_min, y_max, z_min, z_max;
  double cx, cy, cz;     // center of mass
  double mass;           // total mass
  TreeNode* children[8]; // children nodes
  int bodyIndex = -1;         // index of the particle in the bodies array
  bool isLeaf = true;           // is leaf node
};

__global__ void computeBoundingBox(const Bodies& bodies, int n, double* bounds);
__global__ void buildTree(TreeNode* nodes, Bodies& bodies, int n, double x_min, double x_max, double y_min,
                          double y_max, double z_min, double z_max);
__global__ void computeCentersOfMass(TreeNode* nodes, const Bodies& bodies, int n);
__global__ void computeForces(TreeNode* nodes, const Bodies& bodies, double* forces_x, double* forces_y, double* forces_z, int n, double theta);
__global__ void updateBodies(Bodies bodies, int n, double* fx, double* fy, double* fz, double dt);

#endif // BARNESHUT_CUH