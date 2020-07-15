#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"

class Triangle {
public:
    Triangle() : vertices() {};
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3);
    
    Vertex* getVertex(size_t i) { return vertices[i]; }
    void debugPrint() const;

private:
    Vertex* vertices[3];
};

#endif