#ifndef MESH_H_
#define MESH_H_

#include <map>
#include <set>
#include <fstream>
#include <vector>
#include <array>
#include "vertex.h"



class Mesh {
public:
    Mesh(int numVerticesX_, int numVerticesY_, int numVerticesZ_);
    ~Mesh();

    int getNumVerticesX() const { return numVerticesX; }
    int getNumVerticesY() const { return numVerticesY; }
    int getNumVerticesZ() const { return numVerticesZ; }
    void addVertex(VertexIndex index);
    Vertex* getVertex(VertexIndex index) const;

    void triangulate(double surfaceValue, bool blocky);

    void debugPrint2D(int z) const;
    
    void outputVertices(std::ostream& file) const;
    void outputTriangles(std::ostream& file) const;

private:
    struct Triangle {
        Triangle(Point p1, Point p2, Point p3) : p{ p1, p2, p3} {}
        Point p[3];
        std::string getString() const { return std::to_string(p[0].x) + "\t" + std::to_string(p[0].y) + "\t" + std::to_string(p[1].x) + "\t" + std::to_string(p[1].y) + "\t" + std::to_string(p[2].x) + "\t" + std::to_string(p[2].y); }
    };
    
    int vertexArrayIndex(int x, int y, int z) const { return (x + y * numVerticesX + z * numVerticesX * numVerticesY); }
    
    Vertex* vertices;
    
    int numVerticesX;
    int numVerticesY;
    int numVerticesZ;
    int vertex_id_count;

    std::vector<Triangle> triangles;
};

Point avgVertexPosition(const Vertex* v1, const Vertex* v2);


/* 0  4  1
        
   7     5

   3  6  2  

    3210
*/

const std::array<std::array<int, 8>, 16> TRIANGLES_2D_ROUNDED {{
    {-1, -1, -1, -1, -1, -1, -1, -1},   // 0 - 0000
    {0, 4, 7, -1, -1, -1, -1, -1},      // 1 - 0001
    {4, 1, 5, -1, -1, -1, -1, -1},      // 2 - 0010
    {0, 1, 5, 7, -1, -1, -1, -1},       // 3 - 0011
    {5, 2, 6, -1, -1, -1, -1, -1},      // 4 - 0100
    {0, 4, 5, 2, 6, 7, -1, -1},         // 5 - 0101
    {4, 1, 2, 6, -1, -1, -1, -1},       // 6 - 0110
    {0, 1, 2, 6, 7, -1, -1, -1},        // 7 - 0111
    {6, 3, 7, -1, -1, -1, -1, -1},      // 8 - 1000
    {0, 4, 6, 3, -1, -1, -1, -1},       // 8 - 1001
    {4, 1, 5, 6, 3, 7, -1, -1},         // 10 - 1010
    {0, 1, 5, 6, 3, -1, -1, -1},        // 11 - 1011
    {5, 2, 3, 7, -1, -1, -1, -1},       // 12 - 1100
    {0, 4, 5, 2, 3, -1, -1, -1},        // 13 - 1101
    {4, 1, 2, 3, 7, -1, -1, -1},        // 14 - 1110
    {0, 1, 2, 3, -1, -1, -1, -1},       // 15 - 1111
}};

const std::array<std::array<int, 8>, 16> TRIANGLES_2D_BLOCKY {{
    {-1, -1, -1, -1, -1, -1, -1, -1},   // 0 - 0000
    {7, 0, 4, 8, -1, -1, -1, -1},      // 1 - 0001
    {4, 1, 5, 8, -1, -1, -1, -1},      // 2 - 0010
    {0, 1, 5, 7, -1, -1, -1, -1},       // 3 - 0011
    {5, 2, 6, 8, -1, -1, -1, -1},      // 4 - 0100
    {0, 4, 5, 2, 6, 7, -1, -1},         // 5 - 0101
    {4, 1, 2, 6, -1, -1, -1, -1},       // 6 - 0110
    {8, 7, 0, 1, 2, 6, -1, -1},        // 7 - 0111
    {6, 3, 7, 8, -1, -1, -1, -1},      // 8 - 1000
    {0, 4, 6, 3, -1, -1, -1, -1},       // 9 - 1001
    {4, 1, 5, 6, 3, 7, -1, -1},         // 10 - 1010
    {8, 6, 3, 0, 1, 5, -1, -1},        // 11 - 1011
    {5, 2, 3, 7, -1, -1, -1, -1},       // 12 - 1100
    {8, 5, 2, 3, 0, 4, -1, -1},        // 13 - 1101
    {8, 4, 1, 2, 3, 7, -1, -1},        // 14 - 1110
    {0, 1, 2, 3, -1, -1, -1, -1},       // 15 - 1111
}};

#endif