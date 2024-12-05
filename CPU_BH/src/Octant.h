#ifndef OCTANT_H
#define OCTANT_H

#include "Body.h"

struct Octant {
    double x, y, z;
    double size;

    Octant(double x_, double y_, double z_, double size_);
    bool contains(const Body& body) const;
    Octant getSubOctant(int index) const;
};

#endif // OCTANT_H