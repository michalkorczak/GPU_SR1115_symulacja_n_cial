#include "physics.h"

void update_velocities(Body& bodies, int n, double dt) {
#pragma omp parallel for schedule(dynamic, 64)
  for (int i = 0; i < n; i++) {
    double ax_i = 0.0, ay_i = 0.0, az_i = 0.0;

    for (int j = i + 1; j < n; j++) {
      double dx = bodies.x[j] - bodies.x[i];
      double dy = bodies.y[j] - bodies.y[i];
      double dz = bodies.z[j] - bodies.z[i];
      double dist = std::sqrt(dx * dx + dy * dy + dz * dz) + 1e-9;
      double F = G * bodies.mass[i] * bodies.mass[j] / (dist * dist);

      double Fx = F * dx / dist;
      double Fy = F * dy / dist;
      double Fz = F * dz / dist;

      // Siły są symetryczne: Fij = -Fji
      ax_i += Fx / bodies.mass[i];
      ay_i += Fy / bodies.mass[i];
      az_i += Fz / bodies.mass[i];

#pragma omp atomic
      bodies.vx[j] -= dt * Fx / bodies.mass[j];
#pragma omp atomic
      bodies.vy[j] -= dt * Fy / bodies.mass[j];
#pragma omp atomic
      bodies.vz[j] -= dt * Fz / bodies.mass[j];
    }

    // Aktualizacja prędkości ciała i
    bodies.vx[i] += dt * ax_i;
    bodies.vy[i] += dt * ay_i;
    bodies.vz[i] += dt * az_i;
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