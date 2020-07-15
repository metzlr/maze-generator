#include <iostream>
#include <cassert>
#include "mesh.h"
#include <iomanip>


// Generate mesh frame
Mesh::Mesh(int width, int length, int height, double step) {
    // 3 Vertices per "node"
    w = width;
    l = length;
    h = height;
    vertex_step = step;
    vertex_count = 0;
    vertices = new Vertex[w*l*h];

    // Generate vertices
    for (int z = 0; z < h; z++) {
        for (int y = 0; y < l; y++) {
            for (int x = 0; x < w; x++) {
                addVertex(VertexIndex(x, y, z));
            }
        }
    }
    /*
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < l; y++) {
            for (int z = 0; z < h; z++) {
                addVertex(VertexIndex(x, y, z));
            }
        }
    }
    */
    /*
    for (int x = 0; x < w-1; x++) {
        for (int y = 0; y < l-1; y++) {
            Vertex* v1 = getVertex(x, y);
            Vertex* v2 = getVertex(x+1, y);
            Vertex* v3 = getVertex(x+1, y+1);
            Vertex* v4 = getVertex(x, y+1);
            addSquare(v1, v2, v3, v4);
        }
    }
    */
}

// Cleanup dynamic memory
Mesh::~Mesh() {
    delete[] vertices;
}

void Mesh::addVertex(VertexIndex index) {
    vertices[vertex_count] = Vertex(Point(index.x * vertex_step, index.y * vertex_step, index.z * vertex_step), index, vertex_count, true);
    vertex_count++;
}

Vertex* Mesh::getVertex(VertexIndex index) const {
    if (index.x >= w || index.y >= l || index.z >= h || index.x < 0 || index.y < 0 || index.z < 0) {
        /* std::cout << "ERROR: Trying to reach vertex that is out of bounds" << std::endl; */
        return nullptr;
    }
    return &vertices[vertexArrayIndex(index.x, index.y, index.z)];
}


void Mesh::debugPrint() const {
    for (int i = 0; i < w*l*h; i++) {
        if (!vertices[i].isFilled()) {
            std::cout << vertices[i].getString() << std::endl;
        }
        
    }
    std::cout << std::endl;
}

void Mesh::output_to_stream(std::ostream& file) const {
    for (int i = 0; i < w*l*h; i++) {
        Vertex* v = &vertices[i];
        file << v->getString() << std::endl;
    }
}
