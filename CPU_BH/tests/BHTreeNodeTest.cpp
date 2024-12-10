#include "gtest/gtest.h"
#include "../src/BHTreeNode.h"

// Test konstrukcji węzła
TEST(BHTreeNodeTest, ConstructorTest) {
    Octant region(0.0, 0.0, 0.0, 10.0);
    BHTreeNode node(region);

    EXPECT_DOUBLE_EQ(node.region.x, 0.0);
    EXPECT_DOUBLE_EQ(node.region.y, 0.0);
    EXPECT_DOUBLE_EQ(node.region.z, 0.0);
    EXPECT_DOUBLE_EQ(node.region.size, 10.0);
    EXPECT_EQ(node.mass, 0.0);
    EXPECT_EQ(node.centerX, 0.0);
    EXPECT_EQ(node.centerY, 0.0);
    EXPECT_EQ(node.centerZ, 0.0);
    EXPECT_EQ(node.body, nullptr);
}

// Test podziału węzła
TEST(BHTreeNodeTest, SubdivideTest) {
    Octant region(0.0, 0.0, 0.0, 10.0);
    BHTreeNode node(region);

    node.subdivide();

    for (int i = 0; i < 8; ++i) {
        EXPECT_NE(node.children[i], nullptr); // Każdy podregion powinien być poprawnie zainicjalizowany
        EXPECT_DOUBLE_EQ(node.children[i]->region.size, 5.0); // Rozmiar każdego podregionu
    }
}
