#include "gtest/gtest.h"
#include "../src/Octant.h"

// Test konstruktora klasy Octant
TEST(OctantTest, ConstructorTest) {
    Octant octant(0.0, 0.0, 0.0, 10.0);

    EXPECT_DOUBLE_EQ(octant.x, 0.0);
    EXPECT_DOUBLE_EQ(octant.y, 0.0);
    EXPECT_DOUBLE_EQ(octant.z, 0.0);
    EXPECT_DOUBLE_EQ(octant.size, 10.0);
}

// Test funkcji contains dla punktu wewnątrz octanta
TEST(OctantTest, ContainsPointInside) {
    Octant octant(0.0, 0.0, 0.0, 10.0);
    Body body(1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0); // Punkt w środku octanta

    EXPECT_TRUE(octant.contains(body));
}

// Test funkcji contains dla punktu na krawędzi octanta
TEST(OctantTest, ContainsPointOnEdge) {
    Octant octant(0.0, 0.0, 0.0, 10.0);
    Body body(1.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0); // Punkt na krawędzi octanta

    EXPECT_TRUE(octant.contains(body));
}

// Test funkcji contains dla punktu poza octantem
TEST(OctantTest, ContainsPointOutside) {
    Octant octant(0.0, 0.0, 0.0, 10.0);
    Body body(1.0, 6.0, 6.0, 6.0, 0.0, 0.0, 0.0); // Punkt poza octantem

    EXPECT_FALSE(octant.contains(body));
}

// Test funkcji getSubOctant
TEST(OctantTest, GetSubOctantTest) {
    Octant octant(0.0, 0.0, 0.0, 10.0);

    Octant subOctant = octant.getSubOctant(0); // Suboctant o indeksie 0
    EXPECT_DOUBLE_EQ(subOctant.x, -2.5);
    EXPECT_DOUBLE_EQ(subOctant.y, -2.5);
    EXPECT_DOUBLE_EQ(subOctant.z, -2.5);
    EXPECT_DOUBLE_EQ(subOctant.size, 5.0);

    Octant subOctant1 = octant.getSubOctant(7); // Suboctant o indeksie 7
    EXPECT_DOUBLE_EQ(subOctant1.x, 2.5);
    EXPECT_DOUBLE_EQ(subOctant1.y, 2.5);
    EXPECT_DOUBLE_EQ(subOctant1.z, 2.5);
    EXPECT_DOUBLE_EQ(subOctant1.size, 5.0);
}
