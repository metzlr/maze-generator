#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include "point.h"

class Vertex {
public:
    Vertex() : point(Point()), id(-1), filled(true) {}
    Vertex(Point p, VertexIndex index_, long id_, bool filled_) : point(p), index(index_), id(id_), filled(filled_) {}
    double x() const { return point.x; }
    double y() const { return point.y; }
    double z() const { return point.z; }
    int getID() const { return id; }
    bool isFilled() const { return filled; }
    VertexIndex getIndex() const { return index; }

    void fill() { filled = true; }
    void unfill() { filled = false; }
    std::string getString() const;

    //bool operator==(const Vertex& rhs) const { return (x_-rhs.x_ < 0.01 && y_-rhs.y_ < 0.01); }
    bool operator==(const Vertex& rhs) const { return (id == rhs.id); }


private:
    Point point;
    VertexIndex index;
    int id;
    bool filled;
};

#endif