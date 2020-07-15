#include <iostream>
#include "triangle.h"


Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}

void Triangle::debugPrint() const {
    std::cout << "TRIANGLE: " << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "    " << vertices[i]->getString() << std::endl;
    }
}