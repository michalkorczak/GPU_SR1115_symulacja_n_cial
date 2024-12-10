#include "../src/Body.h"
#include <gtest/gtest.h>

// Test poprawności konstruktora
TEST(BodyTest, ConstructorInitializesCorrectly) {
    Body body(5.0, 1.0, 2.0, 3.0, 0.1, 0.2, 0.3);
    
    EXPECT_DOUBLE_EQ(body.mass, 5.0);
    EXPECT_DOUBLE_EQ(body.x, 1.0);
    EXPECT_DOUBLE_EQ(body.y, 2.0);
    EXPECT_DOUBLE_EQ(body.z, 3.0);
    EXPECT_DOUBLE_EQ(body.vx, 0.1);
    EXPECT_DOUBLE_EQ(body.vy, 0.2);
    EXPECT_DOUBLE_EQ(body.vz, 0.3);
    EXPECT_DOUBLE_EQ(body.ax, 0.0);
    EXPECT_DOUBLE_EQ(body.ay, 0.0);
    EXPECT_DOUBLE_EQ(body.az, 0.0);
}

// Test modyfikacji pozycji ciała
TEST(BodyTest, PositionCanBeUpdated) {
    Body body(1.0, 1.0, 2.0, 3.0, 0.1, 0.2, 0.3);

    // Zmień pozycję
    body.x = 4.0;
    body.y = 5.0;
    body.z = 6.0;

    EXPECT_DOUBLE_EQ(body.x, 4.0);
    EXPECT_DOUBLE_EQ(body.y, 5.0);
    EXPECT_DOUBLE_EQ(body.z, 6.0);
}

// Test modyfikacji prędkości ciała
TEST(BodyTest, VelocityCanBeUpdated) {
    Body body(1.0, 1.0, 2.0, 3.0, 0.1, 0.2, 0.3);

    // Zmień prędkość
    body.vx = 0.4;
    body.vy = 0.5;
    body.vz = 0.6;

    EXPECT_DOUBLE_EQ(body.vx, 0.4);
    EXPECT_DOUBLE_EQ(body.vy, 0.5);
    EXPECT_DOUBLE_EQ(body.vz, 0.6);
}

// Test modyfikacji akceleracji ciała
TEST(BodyTest, AccelerationCanBeUpdated) {
    Body body(1.0, 1.0, 2.0, 3.0, 0.1, 0.2, 0.3);

    // Zmień akcelerację
    body.ax = 0.7;
    body.ay = 0.8;
    body.az = 0.9;

    EXPECT_DOUBLE_EQ(body.ax, 0.7);
    EXPECT_DOUBLE_EQ(body.ay, 0.8);
    EXPECT_DOUBLE_EQ(body.az, 0.9);
}

// Test masy ciała
TEST(BodyTest, MassCanBeModified) {
    Body body(1.0, 1.0, 2.0, 3.0, 0.1, 0.2, 0.3);

    // Zmień masę
    body.mass = 10.0;

    EXPECT_DOUBLE_EQ(body.mass, 10.0);
}
