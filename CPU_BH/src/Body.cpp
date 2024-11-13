#include "Body.h"

Body::Body(double m, double x_, double y_, double z_, double vx_, double vy_, double vz_)
    : mass(m), x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), ax(0), ay(0), az(0) {}
