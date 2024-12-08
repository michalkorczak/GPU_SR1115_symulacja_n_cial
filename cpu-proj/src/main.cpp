#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <omp.h>
#include <vector>

using json = nlohmann::json;

#define G 6.67430e-11 // Gravitational constant

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

void save_state(const Body& bodies, int n, const std::string& filename, int step, bool append = true) {
  json step_data;
  step_data["step"] = step;
  step_data["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();

  json bodies_array = json::array();
  for (int i = 0; i < n; i++) {
    bodies_array.push_back(bodies.to_json(i));
  }
  step_data["bodies"] = bodies_array;

  json jsonData;

  if (append) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
      try {
        inFile >> jsonData;
        inFile.close();
      } catch (const json::parse_error& e) {
        // empty or invalid
        jsonData = json::array();
      }
    } else {
      // not found
      jsonData = json::array();
    }
  } else {
    // overwrite
    jsonData = json::array();
  }

  jsonData.push_back(step_data);

  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    std::cerr << "Błąd: Nie można otworzyć pliku do zapisu: " << filename << std::endl;
    return;
  }
  
  outFile << jsonData.dump(2);
  outFile.close();
}

void update_velocities(Body& bodies, int n, double dt) {
#pragma omp parallel for schedule(dynamic, 64)
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i != j) {
        double dx = bodies.x[j] - bodies.x[i];
        double dy = bodies.y[j] - bodies.y[i];
        double dz = bodies.z[j] - bodies.z[i];
        double dist = std::sqrt(dx * dx + dy * dy + dz * dz) + 1e-9;
        double F = G * bodies.mass[i] * bodies.mass[j] / (dist * dist);

        double Fx = F * dx / dist;
        double Fy = F * dy / dist;
        double Fz = F * dz / dist;

        bodies.vx[i] += dt * Fx / bodies.mass[i];
        bodies.vy[i] += dt * Fy / bodies.mass[i];
        bodies.vz[i] += dt * Fz / bodies.mass[i];
      }
    }
  }
}

void update_positions(Body& bodies, int n, double dt) {
#pragma omp parallel for schedule(dynamic, 64)
  for (int i = 0; i < n; i++) {
    bodies.x[i] += bodies.vx[i] * dt;
    bodies.y[i] += bodies.vy[i] * dt;
    bodies.z[i] += bodies.vz[i] * dt;
  }
}


int main(const int argc, const char** argv) {
  srand(time(NULL));

  int n = 1000;
  if (argc > 1) {
    n = atoi(argv[1]);
  }

  int steps = 1000;
  if (argc > 2) {
    steps = atoi(argv[2]);
  }

  int saveInterval = 100;
  if (argc > 3) {
    saveInterval = atoi(argv[3]);
  }

  double dt = 0.01;
  if (argc > 4) {
    dt = atof(argv[4]);
  }

  std::string outputFilename = "output.json";
  if (argc > 5) {
    outputFilename = argv[5];
  }

  Body bodies;
  bodies.resize(n);

  for (int i = 0; i < n; i++) {
    bodies.x[i] = rand() / (double)RAND_MAX * 100.0 - 50.0;
    bodies.y[i] = rand() / (double)RAND_MAX * 100.0 - 50.0;
    bodies.z[i] = rand() / (double)RAND_MAX * 100.0 - 50.0;
    bodies.vx[i] = bodies.vy[i] = bodies.vz[i] = 0.0;
    bodies.mass[i] = 1.0 + rand() / (double)RAND_MAX * 9.0;
  }

  save_state(bodies, n, outputFilename, 0, false);

  auto start = std::chrono::high_resolution_clock::now();

  for (int step = 1; step < steps; step++) {
    update_velocities(bodies, n, dt);
    update_positions(bodies, n, dt);

    if (saveInterval > 0 && step % saveInterval == 0) {
      std::cout << "Krok: " << step << "/" << steps << std::endl;
      save_state(bodies, n, outputFilename, step, true); // Dopisuj kolejne kroki
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Czas wykonania: " << duration.count() << " ms" << std::endl;

  return 0;
}