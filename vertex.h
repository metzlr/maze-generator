#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include "point.h"

class Vertex {
public:
    Vertex() : id(-1), value(-1.0) {}
    Vertex(VertexIndex index_, long id_, double value_) : index(index_), id(id_), value(value_) {}
    int getID() const { return id; }
    double getValue() const { return value; }
    VertexIndex getIndex() const { return index; }
    
    void setValue(double val) { value = val; }
    std::string getString() const;

    bool operator==(const Vertex& rhs) const { return (id == rhs.id); }


private:
    VertexIndex index;
    int id;
    double value;
};

#endif