#include "vertex.h"
#include <sstream>
#include <iomanip>
#include <cmath>

std::string Vertex::getString() const {
    std::ostringstream stream;
    stream << "INDEX\t" << index.x << "\t" << index.y << "\t" << index.z << "\tVALUE\t" << value;//"\tPOS\t" << std::fixed << std::setprecision(3) << point.x << "\t" << point.y << "\t" << point.z;
    return stream.str();
}

Point avgVertexPosition(const Vertex* v1, const Vertex* v2) {
    return Point((v1->getIndex().x + v2->getIndex().x)/2.0, (v1->getIndex().y + v2->getIndex().y)/2.0);
}

double indexDistance(VertexIndex i1, VertexIndex i2) {
    int dx = std::abs(i1.x - i2.x);
    int dy = std::abs(i1.y - i2.y);
    int dz = std::abs(i1.z - i2.z);
    return std::sqrt(std::pow(dx, 2) + std::pow(dy, 2) + std::pow(dz, 2));
}
