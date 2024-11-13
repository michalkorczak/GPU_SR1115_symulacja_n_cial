#ifndef BHTREENODE_H
#define BHTREENODE_H

#include <memory>
#include "Body.h"
#include "Octant.h"

class BHTreeNode {
public:
    Octant region;
    std::unique_ptr<Body> body;
    double mass;
    double centerX, centerY, centerZ;
    std::unique_ptr<BHTreeNode> children[8];

    BHTreeNode(const Octant& region_);
    void insert(const Body& newBody);
    void calculateForce(const Body& target, double& fx, double& fy, double& fz) const;

private:
    void updateMassAndCenter(const Body& newBody);
    void subdivide();
    void placeInChild(const Body& b);
};

#endif // BHTREENODE_H