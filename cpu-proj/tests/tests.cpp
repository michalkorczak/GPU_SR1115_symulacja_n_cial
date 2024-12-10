#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"
#include <cstdio>

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

  nlohmann::json to_json(int i) const {
    return {
        {"position", {{"x", x[i]}, {"y", y[i]}, {"z", z[i]}}},
        {"velocity", {{"x", vx[i]}, {"y", vy[i]}, {"z", vz[i]}}},
        {"mass", mass[i]}};
  }
};

void update_velocities(Body& bodies, int n, double dt);
void update_positions(Body& bodies, int n, double dt);
void save_state(const Body& bodies, int n, const std::string& filename, int step, bool append = true);

// --- Testy ---
TEST(BodyTest, ResizeTest) {
  Body bodies;
  bodies.resize(10);

  EXPECT_EQ(bodies.x.size(), 10);
  EXPECT_EQ(bodies.vx.size(), 10);
  EXPECT_EQ(bodies.mass.size(), 10);
}

TEST(PhysicsTest, VelocityUpdateTest) {
  Body bodies;
  bodies.resize(2);

  // Początkowe pozycje i masy
  bodies.x = {0.0, 1.0};
  bodies.y = {0.0, 0.0};
  bodies.z = {0.0, 0.0};
  bodies.vx = {0.0, 0.0};
  bodies.vy = {0.0, 0.0};
  bodies.vz = {0.0, 0.0};
  bodies.mass = {1.0, 1.0};

  double dt = 1.0;
  update_velocities(bodies, 2, dt);

  // Sprawdzamy, czy prędkości są symetryczne (zachowanie zasady Newtona)
  EXPECT_NEAR(bodies.vx[0], -bodies.vx[1], 1e-9);
  EXPECT_NEAR(bodies.vy[0], -bodies.vy[1], 1e-9);
  EXPECT_NEAR(bodies.vz[0], -bodies.vz[1], 1e-9);
}

TEST(PhysicsTest, PositionUpdateTest) {
  Body bodies;
  bodies.resize(1);

  // Ustawienie prędkości początkowych
  bodies.x = {0.0};
  bodies.y = {0.0};
  bodies.z = {0.0};
  bodies.vx = {1.0};
  bodies.vy = {0.0};
  bodies.vz = {0.0};

  double dt = 1.0;
  update_positions(bodies, 1, dt);

  // Sprawdzenie nowej pozycji
  EXPECT_NEAR(bodies.x[0], 1.0, 1e-9);
  EXPECT_NEAR(bodies.y[0], 0.0, 1e-9);
  EXPECT_NEAR(bodies.z[0], 0.0, 1e-9);
}

TEST(SaveStateTest, JsonOutputTest) {
  Body bodies;
  bodies.resize(2);

  // Ustawienie danych dla testu
  bodies.x = {0.0, 1.0};
  bodies.y = {0.0, 0.0};
  bodies.z = {0.0, 0.0};
  bodies.vx = {0.1, -0.1};
  bodies.vy = {0.0, 0.0};
  bodies.vz = {0.0, 0.0};
  bodies.mass = {1.0, 1.0};

  std::string filename = "test_output.json";
  save_state(bodies, 2, filename, 1, false);

  // Wczytanie pliku
  std::ifstream inFile(filename);
  ASSERT_TRUE(inFile.is_open());

  nlohmann::json jsonData;
  inFile >> jsonData;

  // Sprawdzenie, czy dane w pliku są poprawne
  ASSERT_EQ(jsonData.size(), 1);
  ASSERT_EQ(jsonData[0]["step"], 1);
  EXPECT_NEAR(jsonData[0]["bodies"][0]["position"]["x"], 0.0, 1e-9);
  EXPECT_NEAR(jsonData[0]["bodies"][1]["position"]["x"], 1.0, 1e-9);
}

TEST(BoundaryTest, NoBodiesTest) {
  Body bodies;
  bodies.resize(0);

  double dt = 0.01;
  update_velocities(bodies, 0, dt);
  update_positions(bodies, 0, dt);

  EXPECT_EQ(bodies.x.size(), 0);
  EXPECT_EQ(bodies.vx.size(), 0);
}

TEST(BoundaryTest, SingleBodyNoMovement) {
  Body bodies;
  bodies.resize(1);

  // Pojedyncze ciało bez ruchu
  bodies.x[0] = 0.0;
  bodies.y[0] = 0.0;
  bodies.z[0] = 0.0;
  bodies.vx[0] = 0.0;
  bodies.vy[0] = 0.0;
  bodies.vz[0] = 0.0;
  bodies.mass[0] = 1.0;

  double dt = 1.0;
  update_velocities(bodies, 1, dt);
  update_positions(bodies, 1, dt);

  // Pozycja i prędkość nie powinny się zmienić
  EXPECT_NEAR(bodies.x[0], 0.0, 1e-9);
  EXPECT_NEAR(bodies.vx[0], 0.0, 1e-9);
}

TEST(GravityTest, TwoBodiesSimpleGravity) {
  Body bodies;
  bodies.resize(2);

  // Ciała ustawione wzdłuż osi X
  bodies.x = {0.0, 1.0};
  bodies.y = {0.0, 0.0};
  bodies.z = {0.0, 0.0};
  bodies.vx = {0.0, 0.0};
  bodies.vy = {0.0, 0.0};
  bodies.vz = {0.0, 0.0};
  bodies.mass = {1.0, 1.0};

  double dt = 1.0;
  update_velocities(bodies, 2, dt);

  // Siły powinny być równe i przeciwne
  EXPECT_NEAR(bodies.vx[0], G * 1.0 / (1.0 * 1.0), 1e-9);
  EXPECT_NEAR(bodies.vx[1], -G * 1.0 / (1.0 * 1.0), 1e-9);
}

TEST(GravityTest, ThreeBodiesSymmetry) {
  Body bodies;
  bodies.resize(3);

  // Trójkąt równoboczny w płaszczyźnie XY
  bodies.x = {0.0, 1.0, 0.5};
  bodies.y = {0.0, 0.0, std::sqrt(3.0) / 2.0};
  bodies.z = {0.0, 0.0, 0.0};
  bodies.vx = {0.0, 0.0, 0.0};
  bodies.vy = {0.0, 0.0, 0.0};
  bodies.vz = {0.0, 0.0, 0.0};
  bodies.mass = {1.0, 1.0, 1.0};

  double dt = 0.1;
  update_velocities(bodies, 3, dt);

  // Wszystkie ciała powinny pozostać w stanie symetrycznym
  EXPECT_NEAR(bodies.vx[0], -bodies.vx[1], 1e-9);
  EXPECT_NEAR(bodies.vy[0], -bodies.vy[2], 1e-9);
}

TEST(GravityTest, LargeMassDifference) {
  Body bodies;
  bodies.resize(2);

  // Jedno bardzo masywne ciało i jedno lekkie
  bodies.x = {0.0, 100.0};
  bodies.y = {0.0, 0.0};
  bodies.z = {0.0, 0.0};
  bodies.vx = {0.0, 0.0};
  bodies.vy = {0.0, 0.0};
  bodies.vz = {0.0, 0.0};
  bodies.mass = {1e10, 1.0};

  double dt = 1.0;
  update_velocities(bodies, 2, dt);

  // Lekkie ciało powinno przyspieszyć znacznie bardziej
  EXPECT_GT(std::abs(bodies.vx[1]), std::abs(bodies.vx[0]));
}

TEST(IntegrationTest, PositionUpdateLargeSystem) {
  Body bodies;
  bodies.resize(100);

  // Ustawienie ciał w siatce
  for (int i = 0; i < 100; i++) {
    bodies.x[i] = i % 10;
    bodies.y[i] = i / 10;
    bodies.z[i] = 0.0;
    bodies.vx[i] = 0.1 * i;
    bodies.vy[i] = 0.2 * i;
    bodies.vz[i] = 0.0;
    bodies.mass[i] = 1.0;
  }

  double dt = 0.5;
  update_positions(bodies, 100, dt);

  // Sprawdzenie, czy pozycje zmieniają się poprawnie
  for (int i = 0; i < 100; i++) {
    EXPECT_NEAR(bodies.x[i], (i % 10) + 0.1 * i * dt, 1e-9);
    EXPECT_NEAR(bodies.y[i], (i / 10) + 0.2 * i * dt, 1e-9);
  }
}

TEST(SaveStateTest, SaveMultipleSteps) {
  Body bodies;
  bodies.resize(2);

  // Ustawienie początkowych danych
  bodies.x = {0.0, 1.0};
  bodies.y = {0.0, 0.0};
  bodies.z = {0.0, 0.0};
  bodies.vx = {0.0, 0.0};
  bodies.vy = {0.0, 0.0};
  bodies.vz = {0.0, 0.0};
  bodies.mass = {1.0, 1.0};

  std::string filename = "test_output.json";

  save_state(bodies, 2, filename, 0, false);
  update_positions(bodies, 2, 1.0);
  save_state(bodies, 2, filename, 1, true);

  // Wczytanie pliku
  std::ifstream inFile(filename);
  ASSERT_TRUE(inFile.is_open());

  nlohmann::json jsonData;
  inFile >> jsonData;

  // Sprawdzenie liczby zapisanych kroków
  ASSERT_EQ(jsonData.size(), 2);
  EXPECT_EQ(jsonData[0]["step"], 0);
  EXPECT_EQ(jsonData[1]["step"], 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
