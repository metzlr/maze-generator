#ifndef TRIANGLE_VERTEX_H
#define TRIANGLE_VERTEX_H

#include "voxel.h"


struct TriangleVertex {
  TriangleVertex() : p(Point()), v1(nullptr), v2(nullptr), normal(), id(0) {}
  TriangleVertex(Point point, Voxel* v1_, Voxel* v2_, Point normal_, int id_) : p(point), v1(v1_), v2(v2_), normal(normal_), id(id_) {}

  //bool operator==(const TriangleVertex& rhs) const { return (id == rhs.id); }

  Point p;
  Voxel* v1;
  Voxel* v2;
  Point normal;
  int id;
};

class VoxelPairByID {
public:
  bool operator()(std::pair<Voxel*,Voxel*> a, std::pair<Voxel*,Voxel*> b) const {
    // Sort first by the first voxel. If they are the same, sort by second voxel
    return (a.first->getID() < b.first->getID() || 
      (a.first->getID() == b.first->getID() && a.second->getID() < b.second->getID()));
  }
};

#endif
