#include "vertex.h"


struct TriangleVertex {
  TriangleVertex() : p(Point()), v1(nullptr), v2(nullptr), normal(), id(-1) {}
  TriangleVertex(Point point, Vertex* v1_, Vertex* v2_, Point normal_, int id_) : p(point), v1(v1_), v2(v2_), normal(normal_), id(id_) {}

  //bool operator==(const TriangleVertex& rhs) const { return (id == rhs.id); }

  Point p;
  Vertex* v1;
  Vertex* v2;
  Point normal;
  int id;
};

class VertexPairByID {
public:
  bool operator()(std::pair<Vertex*,Vertex*> a, std::pair<Vertex*,Vertex*> b) const {
    // Sort first by the first vertex. If they are the same, sort by second vertex
    return (a.first->getID() < b.first->getID() || 
      (a.first->getID() == b.first->getID() && a.second->getID() < b.second->getID()));
  }
};
