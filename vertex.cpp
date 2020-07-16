#include "vertex.h"
#include <sstream>
#include <iomanip>

std::string Vertex::getString() const {
    std::ostringstream stream;
    stream << "ID\t" << id << "\tINDEX\t" << index.x << "\t" << index.y << "\t" << index.z << "\tVALUE\t" << value;//"\tPOS\t" << std::fixed << std::setprecision(3) << point.x << "\t" << point.y << "\t" << point.z;
    return stream.str();
}