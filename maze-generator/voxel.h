#ifndef VOXEL_H
#define VOXEL_H

#include <iostream>
#include "point.h"

class Voxel {
public:
    Voxel() : value(-1.0), id(-1) {}
    Voxel(VoxelIndex index_, double value_, int id_) : index(index_), value(value_), id(id_) {}
    //int getID() const { return id; }
    double getValue() const { return value; }
    int getID() const { return id; }
    VoxelIndex getIndex() const { return index; }
    void setIndex(VoxelIndex index_) { index = index_; }
    //void setID(int id_) { id = id_; }
    
    void setValue(double val) { value = val; }
    std::string getString() const;

    bool operator==(const Voxel& rhs) const { return (index == rhs.index); }


private:
    VoxelIndex index;
    double value;
    int id;
};

Point avgVoxelPosition(const Voxel* v1, const Voxel* v2);
double indexDistance(VoxelIndex i1, VoxelIndex i2);

#endif