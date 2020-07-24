#ifndef TRIANGLE_VERTEX_H
#define TRIANGLE_VERTEX_H

#include "voxel.h"


struct TriangleVertex {
  TriangleVertex() : p(Point()), v1(nullptr), v2(nullptr), normal(), id(0) {}
  TriangleVertex(Point point, Voxel* v1_, Voxel* v2_, Point normal_, int id_) : p(point), v1(v1_), v2(v2_), normal(normal_), id(id_) {}

  //bool operator==(const TriangleVertex& rhs) const { return (v1->getID() == rhs.v1->getID() && v2->getID() == rhs.v2->getID()); }

  Point p;
  Voxel* v1;
  Voxel* v2;
  Point normal;
  int id;
};

// Voxel pair hasher and equality operator used for triangle vertices hashmap
struct VoxelPairEquality {
  bool operator()(const std::pair<Voxel*,Voxel*>& a, const std::pair<Voxel*,Voxel*>& b) const {
    return (a.first->getID() == b.first->getID() && a.first->getID() == b.first->getID());
  }
};
struct VoxelPairHash {
  std::size_t operator()(const std::pair<Voxel*,Voxel*>& pair) const {
    using std::size_t;
    using std::hash;
    return (hash<int>()(pair.first->getID()) ^ (hash<int>()(pair.second->getID()) << 1));
  }
};


#endif
