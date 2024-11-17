#include "Octant.h"

Octant::Octant(double x_, double y_, double z_, double size_)
    : x(x_), y(y_), z(z_), size(size_) {}

bool Octant::contains(const Body& body) const {
    return (body.x >= x - size / 2 && body.x <= x + size / 2 &&
        body.y >= y - size / 2 && body.y <= y + size / 2 &&
        body.z >= z - size / 2 && body.z <= z + size / 2);
}

Octant Octant::getSubOctant(int index) const {
    double newSize = size / 2;
    double offsetX = ((index & 1) ? newSize : -newSize) / 2;
    double offsetY = ((index & 2) ? newSize : -newSize) / 2;
    double offsetZ = ((index & 4) ? newSize : -newSize) / 2;
    return Octant(x + offsetX, y + offsetY, z + offsetZ, newSize);
}