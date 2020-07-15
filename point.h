#ifndef POINT_H
#define POINT_H


struct Point {
    Point(double x_=-1.0, double y_=-1.0, double z_=-1.0) : x(x_), y(y_), z(z_) {}
    double x;
    double y;
    double z;
};

struct VertexIndex {
    VertexIndex(int x_=-1, int y_=-1, int z_=-1) : x(x_), y(y_), z(z_) {}
    int x;
    int y;
    int z;
};

#endif