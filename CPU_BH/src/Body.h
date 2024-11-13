#ifndef BODY_H
#define BODY_H

struct Body {
    double mass;
    double x, y, z;
    double vx, vy, vz;
    double ax, ay, az;

    Body(double m, double x_, double y_, double z_, double vx_, double vy_, double vz_);
};

#endif // BODY_H
