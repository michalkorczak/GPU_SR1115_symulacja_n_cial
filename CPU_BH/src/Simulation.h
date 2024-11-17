#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "Body.h"
#include "BHTreeNode.h"

void simulate_step(std::vector<Body>& bodies);
void update_body_leapfrog(Body& body, double fx, double fy, double fz);
void calculate_total_energy(const std::vector<Body>& bodies);
BHTreeNode build_bhtree(std::vector<Body>& bodies);

#endif // SIMULATION_H