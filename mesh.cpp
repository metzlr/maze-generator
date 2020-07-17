#include <iostream>
#include <cassert>
#include "mesh.h"
#include <iomanip>
#include <vector>

// Generate mesh frame
Mesh::Mesh(int numVerticesX_, int numVerticesY_, int numVerticesZ_) {
    numVerticesX = numVerticesX_;
    numVerticesY = numVerticesY_;
    numVerticesZ = numVerticesZ_;
    //vertex_id_count = 0;
    //vertices = new Vertex[numVerticesX*numVerticesY*numVerticesZ];
    
    // Generate vertices
    for (int z = 0; z < numVerticesZ; z++) {
        for (int y = 0; y < numVerticesY; y++) {
            for (int x = 0; x < numVerticesX; x++) {
                vertices.push_back(new Vertex(VertexIndex(x, y, z), -1.0));
                //addVertex(VertexIndex(x, y, z));
            }
        }
    }
}

// Cleanup dynamic memory
Mesh::~Mesh() {
    for (size_t i = 0; i < vertices.size(); i++) {
        delete vertices[i];
    }
}

// void Mesh::addVertex(VertexIndex index) {
//     vertices.push_back(new Vertex(index,  -1.0));
//     vertex_id_count++;
// }

Vertex* Mesh::getVertex(VertexIndex index) const {
    if (index.x >= numVerticesX || index.y >= numVerticesY || index.z >= numVerticesZ || index.x < 0 || index.y < 0 || index.z < 0) {
        /* std::cout << "ERROR: Trying to reach vertex that is out of bounds" << std::endl; */
        return nullptr;
    }
    return vertices[vertexArrayIndex(index.x, index.y, index.z)];
}

void Mesh::outputVertices(std::ostream& stream) const {
    for (int i = 0; i < numVerticesX*numVerticesY*numVerticesZ; i++) {
        Vertex* v = vertices[i];
        stream << v->getString() << std::endl;
    }
}

void Mesh::outputTriangles(std::ostream& stream) const {
    for (unsigned int i = 0; i < triangles.size(); i++) {
        stream << triangles[i].getString() << std::endl;
    }
}


std::vector<const Vertex*> Mesh::getCell(VertexIndex index, bool cube) const {
    std::vector<const Vertex*> vertices;
    vertices.push_back(getVertex(index));
    vertices.push_back(getVertex(VertexIndex(index.x + 1, index.y, index.z)));
    vertices.push_back(getVertex(VertexIndex(index.x + 1, index.y + 1, index.z)));
    vertices.push_back(getVertex(VertexIndex(index.x, index.y + 1, index.z)));
    if (cube) {
        vertices.push_back(getVertex(VertexIndex(index.x, index.y, index.z + 1)));
        vertices.push_back(getVertex(VertexIndex(index.x + 1, index.y, index.z + 1)));
        vertices.push_back(getVertex(VertexIndex(index.x + 1, index.y + 1, index.z + 1)));
        vertices.push_back(getVertex(VertexIndex(index.x, index.y + 1, index.z + 1)));
    }
    return vertices;
}

double getWeightedValue(double value1, double value2, double surface_cutoff) {
    double distance = std::abs(value1 - value2);
    double from_surface = std::abs(surface_cutoff - value1);
    return (from_surface/distance);
}

void Mesh::triangulate(double surfaceValue, bool blocky) {
    triangles.clear();
    if (numVerticesZ > 1) {     // 3D Mesh
        return;
    } else {                    // 2D Mesh
        for (int y = 0; y < numVerticesY-1; y++) {
            for (int x = 0; x < numVerticesX-1; x++) {
                // Corner vertices
                //const Vertex* corners[4] = { getVertex(VertexIndex(x, y, 0)), getVertex(VertexIndex(x+1, y, 0)), getVertex(VertexIndex(x+1, y+1, 0)), getVertex(VertexIndex(x, y+1, 0)) };
                std::vector<const Vertex*> corners = getCell(VertexIndex(x, y, 0), false);
                // Get triangle index from active points
                int triangleIndex = 0;
                if (corners[0]->getValue() < surfaceValue) triangleIndex |= 1;
                if (corners[1]->getValue() < surfaceValue) triangleIndex |= 2;
                if (corners[2]->getValue() < surfaceValue) triangleIndex |= 4;
                if (corners[3]->getValue() < surfaceValue) triangleIndex |= 8;
                if (triangleIndex == 0) continue; // No active corners

                // Get vertices from list and create list of points
                std::vector<Point> points;
                const std::array<std::array<int, 8>, 16> * triangleArray;
                if (blocky)
                    triangleArray = &TRIANGLES_2D_BLOCKY;
                else
                    triangleArray = &TRIANGLES_2D_ROUNDED;

                for (int i = 0; (*triangleArray)[triangleIndex][i] != -1; i++) {
                    int vertexIndex = (*triangleArray)[triangleIndex][i];
                    Point p;
                    if (vertexIndex < 4) {
                        p = Point(corners[vertexIndex]->getIndex().x, corners[vertexIndex]->getIndex().y);
                    } else if (vertexIndex == 4) {
                        // p = avgVertexPosition(corners[0], corners[1]);
                        double val = getWeightedValue(corners[0]->getValue(), corners[1]->getValue(), surfaceValue);
                        p = Point(corners[0]->getIndex().x + val, corners[0]->getIndex().y);
                    } else if (vertexIndex == 5) {
                        // p = avgVertexPosition(corners[1], corners[2]);
                        double val = getWeightedValue(corners[1]->getValue(), corners[2]->getValue(), surfaceValue);
                        p = Point(corners[1]->getIndex().x, corners[1]->getIndex().y + val);
                    } else if (vertexIndex == 6) {
                        // p = avgVertexPosition(corners[2], corners[3]);
                        double val = getWeightedValue(corners[2]->getValue(), corners[3]->getValue(), surfaceValue);
                        p = Point(corners[2]->getIndex().x + -1 * val, corners[2]->getIndex().y);
                    } else if (vertexIndex == 7) {
                        // p = avgVertexPosition(corners[3], corners[0]);
                        double val = getWeightedValue(corners[3]->getValue(), corners[0]->getValue(), surfaceValue);
                        p = Point(corners[3]->getIndex().x, corners[3]->getIndex().y + -1 * val);
                    } else if (vertexIndex == 8 && blocky) {
                        p = avgVertexPosition(corners[0], corners[2]);
                        //double val = getWeightedValue(corners[0]->getValue(), corners[1]->getValue(), surfaceValue);
                        //p = Point(corners[0]->getIndex().x * val, corners[0]->getIndex().y);
                    } else {
                        std::cerr << "Error: Unknown vertex index " << vertexIndex << std::endl;
                        return;
                    }
                    points.push_back(p);
                }

                // Get triangles from list of points
                if (points.size() >= 3) {
                    triangles.push_back(Triangle(points[0], points[1], points[2]));
                }
                if (points.size() >= 4) {
                    triangles.push_back(Triangle(points[0], points[2], points[3]));
                }
                if (points.size() >= 5) {
                    triangles.push_back(Triangle(points[0], points[3], points[4]));
                }
                if (points.size() >= 6) {
                    triangles.push_back(Triangle(points[0], points[4], points[5]));
                }
            }
        }
    }
}

void Mesh::recursiveBlend(VertexIndex index, VertexIndex center, double weight, double radius, std::vector<VertexIndex> &already_set) {
    if (index.x >= numVerticesX || index.y >= numVerticesY || index.z >= numVerticesZ || index.x < 0 || index.y < 0 || index.z < 0) return;
    for (size_t i = 0; i < already_set.size(); i++) {
        if (already_set[i] == index) return;
    }
    //if (getVertex(index)->getValue() != -1.0 && !initial) return;
    double distance = indexDistance(index, center);
    
    if (distance != 0.0) {
        if (lessThanOrEqual(distance, radius)) {
            already_set.push_back(index);
            double value = weight/distance;
            if (getVertex(index)->getValue() < value) { getVertex(index)->setValue(value); }
        } else { return; }
    }
    recursiveBlend(VertexIndex(index.x, index.y + 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y - 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x + 1, index.y, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x - 1, index.y, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y, index.z + 1), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y, index.z - 1), center, weight, radius, already_set);
}

void Mesh::blend(double radius, double weight) {
    if (radius <= 0) return;
    for (int z = 0; z < numVerticesZ; z++) {
        for (int y = 0; y < numVerticesY; y++) {
            for (int x = 0; x < numVerticesX; x++) {
                VertexIndex index = VertexIndex(x, y, z);
                Vertex* v = getVertex(index);
                if (v->getValue() < 1.0) continue;
                std::vector<VertexIndex> already_set;
                recursiveBlend(index, index, weight, radius, already_set);
                already_set.empty();
            }
        }
    }
}

/* 


void recursive_split(Vertex v1, Vertex v2, int num_divides, std::vector<Vertex>& vertices, int direction, bool initial=true) {
    if (initial) vertices.push_back(v1);
    if (num_divides > 0) {
        //Point midPos = avgVertexPosition(&v1, &v2);
        VertexIndex midIndex = v1.getIndex();
        //midIndex.x += 1;
        Vertex midVertex(midIndex, avgValue(v1.getValue(), v2.getValue()));
        recursive_split(v1, midVertex, num_divides-1, vertices, false);
        recursive_split(midVertex, v2, num_divides-1, vertices, false);
    } else {
        VertexIndex index;
        if (direction == 0) {           // X split
            index = VertexIndex(vertices[0].getIndex().x + vertices.size(), v2.getIndex().y, v2.getIndex().z);
        } else if (direction == 1) {    // Y split
            index = VertexIndex(v2.getIndex().x, vertices[0].getIndex().y + vertices.size(), v2.getIndex().z);
        } else {                        // Z split
            index = VertexIndex(v2.getIndex().x, v2.getIndex().y, vertices[0].getIndex().z + vertices.size());
        }
        v2.setIndex(index);
        vertices.push_back(v2);
    }
}


void Mesh::increaseResolution(int divides) {
    int factor = 2^divides;
    //int newZ = 1;
    //if (numVerticesZ > 1) { newZ = numVerticesZ * factor - 1; }
    //int newX = numVerticesX * factor - 1;
    //int newY = numVerticesZ * factor - 1;
    //Vertex* new_vertices = new Vertex[newZ * newY * newX];
    std::vector<Vertex> new_vertices;
    std::vector<Vertex> block;
    //int vertex_count = 0;
    for (int z = 0; z < (numVerticesZ == 1 ? 1 : numVerticesZ - 1); z++) {
        for (int y = 0; y < numVerticesY - 1; y++) {
            for (int x = 0; x < numVerticesX - 1; x++) {
                std::vector<const Vertex*> cell_vertices = getCell(VertexIndex(x,y,x), false);
                block.clear();
                recursive_split(*cell_vertices[0], *cell_vertices[1], 1, block, 0);
                for (unsigned i = 0; i < block.size(); i++) {
                    //block[i].setID(vertex_count);
                    new_vertices.push_back(block[i]);
                    //new_vertices[vertex_count] = block[i];
                    //std::cout << new_vertices[vertex_count].getString() << std::endl;
                    //vertex_count++;
                }
            }
        }
    }
    for (int i = 0; i < new_vertices.size(); i++) {
        std::cout << new_vertices[i].getString() << std::endl;
    }
} */

void Mesh::debugPrint2D(int z) const {
    for (int y = 0; y < numVerticesY; y++) {
        for (int x = 0; x < numVerticesX; x++) {
            Vertex* v = getVertex(VertexIndex(x, y, z));
            char fill = ' ';
            if (v->getValue() < 0.0) fill = '+';
            std::cout << fill << " ";
        }
        std::cout << std::endl;
    }
}

void Mesh::debugPrintValues(int z) const {
    for (int y = 0; y < numVerticesY; y++) {
        for (int x = 0; x < numVerticesX; x++) {
            Vertex* v = getVertex(VertexIndex(x, y, z));
            std::cout << std::setw(3) << v->getValue();
        }
        std::cout << std::endl;
    }
}