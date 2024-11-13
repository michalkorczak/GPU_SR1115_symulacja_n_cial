#include <iostream>
#include <vector>
#include "Body.h"
#include "Simulation.h"

int main() {
    std::vector<Body> bodies = {
    Body(1.0e24, 500.0, 500.0, 0.0, -1.0, -1.0, 0.0),  
    Body(1.0e24, -500.0, 500.0, 0.0, 1.0, -1.0, 0.0),  
    Body(1.0e24, -500.0, -500.0, 0.0, 1.0, 1.0, 0.0),  
    Body(1.0e24, 500.0, -500.0, 0.0, -1.0, 1.0, 0.0),
    };

    int steps = 100;

    // G³ówna pêtla symulacji
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
