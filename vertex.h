#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include "point.h"

class Vertex {
public:
    Vertex() : value(-1.0) {}
    Vertex(VertexIndex index_, double value_) : index(index_), value(value_) {}
    //int getID() const { return id; }
    double getValue() const { return value; }
    VertexIndex getIndex() const { return index; }
    void setIndex(VertexIndex index_) { index = index_; }
    //void setID(int id_) { id = id_; }
    
    void setValue(double val) { value = val; }
    std::string getString() const;

    bool operator==(const Vertex& rhs) const { return (index == rhs.index); }


private:
    VertexIndex index;
    double value;
};

Point avgVertexPosition(const Vertex* v1, const Vertex* v2);
double indexDistance(VertexIndex i1, VertexIndex i2);

#endif