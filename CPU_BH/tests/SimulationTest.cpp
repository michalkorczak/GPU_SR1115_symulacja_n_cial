#include "gtest/gtest.h"
#include "../src/Simulation.h"
#include "../src/Body.h"
#include <vector>
#include <cmath>

const double G = 6.67430e-11;

// Test funkcji update_body_leapfrog
TEST(SimulationTest, UpdateBodyLeapfrogTest) {
    // Tworzymy ciało z określoną pozycją, prędkością i masą
    Body body(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

    // Siły działające na ciało (przykładowe wartości)
    double fx = 1.0, fy = 0.0, fz = 0.0;

    // Zapamiętujemy wartości przed aktualizacją
    double initialX = body.x;
    double initialY = body.y;
    double initialZ = body.z;
    double initialVx = body.vx;
    double initialVy = body.vy;
    double initialVz = body.vz;

    // Wykonujemy aktualizację
    update_body_leapfrog(body, fx, fy, fz);

    // Sprawdzamy, czy pozycja i prędkość zostały zaktualizowane
    EXPECT_NE(body.x, initialX);
    EXPECT_NE(body.y, initialY);
    EXPECT_NE(body.z, initialZ);
    EXPECT_NE(body.vx, initialVx);
    EXPECT_NE(body.vy, initialVy);
    EXPECT_NE(body.vz, initialVz);
}

// Test funkcji simulate_step
TEST(SimulationTest, SimulateStepTest) {
    // Tworzymy wektor ciał z przykładowymi danymi
    std::vector<Body> bodies = {
        Body(1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0),
        Body(2.0, 2.0, 2.0, 2.0, 0.0, 0.0, 0.0),
        Body(3.0, 3.0, 3.0, 3.0, 0.0, 0.0, 0.0)
    };

    // Wykonujemy krok symulacji
    simulate_step(bodies);

    // Sprawdzamy, czy pozycje ciał zostały zmienione
    for (const auto& body : bodies) {
        EXPECT_NE(body.x, 1.0);
        EXPECT_NE(body.y, 1.0);
        EXPECT_NE(body.z, 1.0);
    }
}

// Test funkcji calculate_total_energy
TEST(SimulationTest, CalculateTotalEnergyTest) {
    // Tworzymy wektor ciał z przykładowymi danymi
    std::vector<Body> bodies = {
        Body(1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0),
        Body(2.0, 2.0, 2.0, 1.0, 0.0, 1.0, 0.0),
        Body(3.0, 3.0, 3.0, 1.0, 0.0, 0.0, 1.0)
    };

    // Obliczamy energię przed symulacją
    double kinetic_total = 0.0, potential_total = 0.0;
    for (const auto& body : bodies) {
        double speed_sq = body.vx * body.vx + body.vy * body.vy + body.vz * body.vz;
        kinetic_total += 0.5 * body.mass * speed_sq;
    }

    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            double dx = bodies[i].x - bodies[j].x;
            double dy = bodies[i].y - bodies[j].y;
            double dz = bodies[i].z - bodies[j].z;
            double dist = std::sqrt(dx * dx + dy * dy + dz * dz);
            potential_total -= G * bodies[i].mass * bodies[j].mass / dist;
        }
    }

    double expected_energy = kinetic_total + potential_total;

    // Obliczamy energię po symulacji
    calculate_total_energy(bodies);

    // Testujemy, czy energia całkowita nie jest zerowa
    EXPECT_NEAR(kinetic_total + potential_total, expected_energy, 1e-10);
}