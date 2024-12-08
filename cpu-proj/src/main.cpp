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
  double x, y, z;
  double vx, vy, vz;
  double mass;

  json to_json() const {
    return {
        {"position", {{"x", x}, {"y", y}, {"z", z}}}, {"velocity", {{"x", vx}, {"y", vy}, {"z", vz}}}, {"mass", mass}};
  }
};

void save_state(const std::vector<Body>& bodies, const std::string& filename, int step, bool append = true) {
  json step_data;
  step_data["step"] = step;
  step_data["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();

  json bodies_array = json::array();
  for (const auto& body : bodies) {
    bodies_array.push_back(body.to_json());
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

void update_velocities(std::vector<Body>& bodies, double dt) {
  int n = bodies.size();

#pragma omp parallel for
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i != j) {
        double dx = bodies[j].x - bodies[i].x;
        double dy = bodies[j].y - bodies[i].y;
        double dz = bodies[j].z - bodies[i].z;
        double dist = std::sqrt(dx * dx + dy * dy + dz * dz) + 1e-9;
        double F = G * bodies[i].mass * bodies[j].mass / (dist * dist);

        bodies[i].vx += dt * F * dx / (dist * bodies[i].mass);
        bodies[i].vy += dt * F * dy / (dist * bodies[i].mass);
        bodies[i].vz += dt * F * dz / (dist * bodies[i].mass);
      }
    }
  }
}

void update_positions(std::vector<Body>& bodies, double dt) {
#pragma omp parallel for
  for (int i = 0; i < static_cast<int>(bodies.size()); ++i) {
    bodies[i].x += bodies[i].vx * dt;
    bodies[i].y += bodies[i].vy * dt;
    bodies[i].z += bodies[i].vz * dt;
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

  std::vector<Body> bodies(n);

  for (auto& body : bodies) {
    body.x = rand() / (double)RAND_MAX * 100.0 - 50.0;
    body.y = rand() / (double)RAND_MAX * 100.0 - 50.0;
    body.z = rand() / (double)RAND_MAX * 100.0 - 50.0;
    body.vx = body.vy = body.vz = 0.0;
    body.mass = 1.0 + rand() / (double)RAND_MAX * 9.0;
  }

  save_state(bodies, outputFilename, 0, false);

  auto start = std::chrono::high_resolution_clock::now();

  for (int step = 1; step < steps; step++) {
    update_velocities(bodies, dt);
    update_positions(bodies, dt);

    if (saveInterval > 0 && step % saveInterval == 0) {
      std::cout << "Krok: " << step << "/" << steps << std::endl;
      save_state(bodies, outputFilename, step, true); // Dopisuj kolejne kroki
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Czas wykonania: " << duration.count() << " ms" << std::endl;

  return 0;
}