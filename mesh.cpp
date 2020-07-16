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
    vertex_id_count = 0;
    vertices = new Vertex[numVerticesX*numVerticesY*numVerticesZ];
    
    // Generate vertices
    for (int z = 0; z < numVerticesZ; z++) {
        for (int y = 0; y < numVerticesY; y++) {
            for (int x = 0; x < numVerticesX; x++) {
                addVertex(VertexIndex(x, y, z));
            }
        }
    }
}

// Cleanup dynamic memory
Mesh::~Mesh() {
    delete[] vertices;
}

void Mesh::addVertex(VertexIndex index) {
    vertices[vertex_id_count] = Vertex(index, vertex_id_count, -1.0);
    vertex_id_count++;
}

Vertex* Mesh::getVertex(VertexIndex index) const {
    if (index.x >= numVerticesX || index.y >= numVerticesY || index.z >= numVerticesZ || index.x < 0 || index.y < 0 || index.z < 0) {
        /* std::cout << "ERROR: Trying to reach vertex that is out of bounds" << std::endl; */
        return nullptr;
    }
    return &vertices[vertexArrayIndex(index.x, index.y, index.z)];
}

void Mesh::outputVertices(std::ostream& stream) const {
    for (int i = 0; i < numVerticesX*numVerticesY*numVerticesZ; i++) {
        Vertex* v = &vertices[i];
        stream << v->getString() << std::endl;
    }
}

void Mesh::outputTriangles(std::ostream& stream) const {
    for (unsigned int i = 0; i < triangles.size(); i++) {
        stream << triangles[i].getString() << std::endl;
    }
}

Point avgVertexPosition(const Vertex* v1, const Vertex* v2) {
    return Point((v1->getIndex().x + v2->getIndex().x)/2.0, (v1->getIndex().y + v2->getIndex().y)/2.0);
}

void Mesh::triangulate(double surfaceValue, bool blocky) {
    triangles.clear();
    if (numVerticesZ > 1) {     // 3D Mesh
        return;
    } else {                    // 2D Mesh
        for (int y = 0; y < numVerticesY-1; y++) {
            for (int x = 0; x < numVerticesX-1; x++) {
                // Corner vertices
                const Vertex* corners[4] = { getVertex(VertexIndex(x, y, 0)), getVertex(VertexIndex(x+1, y, 0)), getVertex(VertexIndex(x+1, y+1, 0)), getVertex(VertexIndex(x, y+1, 0)) };
                
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
                        p = avgVertexPosition(corners[0], corners[1]);
                    } else if (vertexIndex == 5) {
                        p = avgVertexPosition(corners[1], corners[2]);
                    } else if (vertexIndex == 6) {
                        p = avgVertexPosition(corners[2], corners[3]);
                    } else if (vertexIndex == 7) {
                        p = avgVertexPosition(corners[3], corners[0]);
                    } else if (vertexIndex == 8 && blocky) {
                        p = avgVertexPosition(corners[0], corners[2]);
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