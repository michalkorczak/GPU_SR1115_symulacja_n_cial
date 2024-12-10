#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include "Body.h"
#include "Simulation.h"

/*
// Funkcja do generowania losowych ciał
std::vector<Body> generate_random_bodies(int n) {
    std::vector<Body> bodies;
    bodies.reserve(n);

    for (int i = 0; i < n; ++i) {
        double mass = 1.0e24 + rand() / (double)RAND_MAX * 1.0e25;
        double x = rand() / (double)RAND_MAX * 1000.0 - 500.0;
        double y = rand() / (double)RAND_MAX * 1000.0 - 500.0;
        double z = rand() / (double)RAND_MAX * 1000.0 - 500.0;
        double vx = rand() / (double)RAND_MAX * 2.0 - 1.0;
        double vy = rand() / (double)RAND_MAX * 2.0 - 1.0;
        double vz = rand() / (double)RAND_MAX * 2.0 - 1.0;

        bodies.emplace_back(mass, x, y, z, vx, vy, vz);
    }

    return bodies;
}

// TESTOWANIE
int main() {
    // Zakres wartości N
    const int minN = 10;
    const int maxN = 5000;
    const int stepSize = 10;

    int steps = 100; // Liczba kroków symulacji

    // Plik do zapisu wyników
    std::ofstream resultsFile("barnes_hut_results.csv");
    if (!resultsFile.is_open()) {
        std::cerr << "Błąd: Nie można otworzyć pliku do zapisu wyników." << std::endl;
        return 1;
    }

    resultsFile << "N,ExecutionTime(ms)\n";

    for (int n = minN; n <= maxN; n += stepSize) {
        std::vector<Body> bodies = generate_random_bodies(n);

        auto start = std::chrono::high_resolution_clock::now();

        // Symulacja
        for (int step = 0; step < steps; ++step) {
            simulate_step(bodies);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // Zapis wyników
        std::cout << "N: " << n << ", Czas wykonania: " << duration.count() << " ms" << std::endl;
        resultsFile << n << "," << duration.count() << "\n";
    }

    resultsFile.close();

    return 0;
}
*/


int main() {
    std::vector<Body> bodies = {
    Body(1.0e24, 500.0, 500.0, 0.0, -1.0, -1.0, 0.0),  
    Body(1.0e24, -500.0, 500.0, 0.0, 1.0, -1.0, 0.0),  
    Body(1.0e24, -500.0, -500.0, 0.0, 1.0, 1.0, 0.0),  
    Body(1.0e24, 500.0, -500.0, 0.0, -1.0, 1.0, 0.0),
    };

    int steps = 100;

    // Glowna petla symulacji
    for (int step = 0; step < steps; ++step) {
        simulate_step(bodies);
        calculate_total_energy(bodies);

        if (step % 10 == 0) {
            std::cout << "Krok " << step << ":\n";
            for (const auto& body : bodies) {
                std::cout << "Cialo: x=" << body.x << " y=" << body.y << " z=" << body.z
                    << " vx=" << body.vx << " vy=" << body.vy << " vz=" << body.vz << "\n";
            }
        }
    }

    char x;
    std::cout << "Wcisnij dowolny klawisz, aby zamknac";
    std::cin >> x;

    return 0;
}