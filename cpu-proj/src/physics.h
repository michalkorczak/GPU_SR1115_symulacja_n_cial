#pragma once
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;
#define G 6.67430e-11 

struct Body {
  std::vector<double> x, y, z;
  std::vector<double> vx, vy, vz;
  std::vector<double> mass;

  void resize(int n) {
    x.resize(n);
    y.resize(n);
    z.resize(n);
    vx.resize(n);
    vy.resize(n);
    vz.resize(n);
    mass.resize(n);
  }

  json to_json(int i) const {
    return {
        {"position", {{"x", x[i]}, {"y", y[i]}, {"z", z[i]}}},
        {"velocity", {{"x", vx[i]}, {"y", vy[i]}, {"z", vz[i]}}},
        {"mass", mass[i]}};
  }
};

void update_velocities(Body &body, int param, double dt);
void update_positions(Body &body, int param, double dt);
void save_state(const Body &body, int param, const std::string &filename, int mode, bool append);