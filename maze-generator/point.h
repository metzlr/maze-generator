#ifndef POINT_H
#define POINT_H

#include <cmath>

struct Point {
    Point(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}
    double x;
    double y;
    double z;
};

struct VoxelIndex {
    VoxelIndex(int x_=-1, int y_=-1, int z_=-1) : x(x_), y(y_), z(z_) {}
    int x;
    int y;
    int z;
    bool operator==(const VoxelIndex& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }
};

inline double avgValue(double v1, double v2) { return (v1+v2)/2; }
inline bool lessThanOrEqual(double d1, double d2) { return (d1 < d2 || std::abs(d2 - d1) < 0.0001); }

#endif