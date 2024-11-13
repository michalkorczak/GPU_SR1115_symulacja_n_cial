#include "Simulation.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <omp.h>

const double G = 6.67430e-11;
const double dt = 0.01; // krok czasowy

// aktualizuje pozycję i prędkość ciała metodą Leapfrog, biorąc pod uwagę siły
void update_body_leapfrog(Body& body, double fx, double fy, double fz) {
    // obliczanie przyspieszenia na podstawie siły
    body.ax = fx / body.mass;
    body.ay = fy / body.mass;
    body.az = fz / body.mass;

    // aktualizacji prędkości w połowie kroku
    body.vx += body.ax * dt * 0.5;
    body.vy += body.ay * dt * 0.5;
    body.vz += body.az * dt * 0.5;

    // aktualizacja pozycji ciała
    body.x += body.vx * dt;
    body.y += body.vy * dt;
    body.z += body.vz * dt;

    // aktualizacja prędkości w drugiej połowie kroku
    body.vx += body.ax * dt * 0.5;
    body.vy += body.ay * dt * 0.5;
    body.vz += body.az * dt * 0.5;
}

// pojedynyczy krok symulacyjny
void simulate_step(std::vector<Body>& bodies) {
    BHTreeNode root = build_bhtree(bodies);

    // obliczanie siły na każde ciało równolegle
    #pragma omp parallel for 
    for (int i = 0; i < bodies.size(); ++i) {
        double fx = 0.0, fy = 0.0, fz = 0.0;
        root.calculateForce(bodies[i], fx, fy, fz);
        update_body_leapfrog(bodies[i], fx, fy, fz);
    }
}

void calculate_total_energy(const std::vector<Body>& bodies) {
    double kinetic_total = 0.0;
    double potential_total = 0.0;

    // Energia kinetyczna
    for (const auto& body : bodies) {
        double speed_sq = body.vx * body.vx + body.vy * body.vy + body.vz * body.vz;
        kinetic_total += 0.5 * body.mass * speed_sq;
    }

    // Energia potencjalna
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            double dx = bodies[i].x - bodies[j].x;
            double dy = bodies[i].y - bodies[j].y;
            double dz = bodies[i].z - bodies[j].z;
            double dist = std::sqrt(dx * dx + dy * dy + dz * dz);
            potential_total -= G * bodies[i].mass * bodies[j].mass / dist;
        }
    }

    double total_energy = kinetic_total + potential_total;
  //  std::cout << std::fixed << std::setprecision(10) << "Energia kinetyczna: " << kinetic_total << " J, Energia potencjalna: " << potential_total << " J, Calkowita energia: " << total_energy << " J\n";
}

// budowanie drzewa Barnes-Hut
BHTreeNode build_bhtree(std::vector<Body>& bodies) {
    // znajdowanie minimalnych i maksymalnych wartości pozycji dla ograniczenia przestrzeni
    double minX = bodies[0].x, maxX = bodies[0].x;
    double minY = bodies[0].y, maxY = bodies[0].y;
    double minZ = bodies[0].z, maxZ = bodies[0].z;

    for (const auto& body : bodies) {
        if (body.x < minX) minX = body.x;
        if (body.x > maxX) maxX = body.x;
        if (body.y < minY) minY = body.y;
        if (body.y > maxY) maxY = body.y;
        if (body.z < minZ) minZ = body.z;
        if (body.z > maxZ) maxZ = body.z;
    }

    // definiuje oktant o rozmiarze dostosowanym do przestrzeni
    double world_size = std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ);
    Octant root_region((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2, world_size * 1.5);
    BHTreeNode root(root_region);

    // wstawia każde ciało do drzewa
    for (const auto& body : bodies) {
        root.insert(body);
    }

    return root;
}
