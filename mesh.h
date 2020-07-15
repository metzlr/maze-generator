#ifndef MESH_H_
#define MESH_H_

#include <map>
#include <set>
#include <fstream>
#include "vertex.h"


//typedef std::map<std::pair<Vertex*, Vertex*>, Edge*, VertexPairByID> edge_map;

class Mesh {
public:
    Mesh(int width, int length, int height, double step);
    ~Mesh();

    //MazeNode* getSquare(int x, int y) const;

    int width() const { return w; }
    int length() const { return l; }
    int height() const { return h; }
    void addVertex(VertexIndex index);
    Vertex* getVertex(VertexIndex index) const;
    //VertexIndex vertexPositionInArray(int id) const { return VertexIndex(id / l, id % l, 0); }

    void debugPrint() const;

    void output_to_stream(std::ostream& file) const;

private:
    
    //int vertexArrayIndex(int x, int y, int z) const { return (x * l + y); }
    int vertexArrayIndex(int x, int y, int z) const { return (x + y * w + z * w * l); }
    //MazeNode* addNode(Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);
    //Edge* addEdge(Vertex* v1, Vertex* v2);
    //Vertex* addVertex(double x, double y);
    
    //int squareArrayIndex(int x, int y) const { return arrayIndex(x,y,l); }
    //int vertexArrayIndex(int x, int y) const { return arrayIndex(x,y,l+1); }
    
    Vertex* vertices;
    
    int w;
    int l;
    int h;
    int vertex_count;
    double vertex_step;
};

#endif