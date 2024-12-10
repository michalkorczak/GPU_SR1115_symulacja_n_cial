#include <iostream>
#include "physics.h"

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