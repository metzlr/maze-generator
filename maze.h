#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <queue>
#include "mesh.h"


class Maze {
public:

    Maze(int w=10, int l=10, int h=0, double step=1.0, unsigned long seed_=(unsigned int)time(NULL));

    unsigned int getSeed() const { return seed; }
    std::string getString() const;
    

    int debug_simple_print() const;
    void output_to_stream(std::ostream& file) const;

    int getWidth() const { return width; }
    int getLength() const { return length; }
    int getHeight() const { return height; }

private:
    struct Move {
        //Move() : maze(nullptr) {}
        //Move(Maze* m) : maze(m) {}
        inline void addIndex(VertexIndex index, bool active) {
            if (active) potential_active.push_back(index);
            fill.push_back(index);
        }
        //Vertex* selectMove();
        std::vector<VertexIndex> getFill() const { return fill; }
        std::vector<VertexIndex> getPotentialActive() const { return potential_active; }
    private:
        //Maze* maze;
        std::vector<VertexIndex> fill;
        std::vector<VertexIndex> potential_active;
    };

    std::vector<Move> getPossibleMovesSquare(Vertex* v);
    bool generatePath(const std::vector<int>& sizes);
    std::vector<int> generatePathLengths(bool initial) const;

    Mesh* mesh;
    std::queue<Vertex*> active_vertices;

    unsigned int seed;
    int width;
    int length;
    int height;
};


#endif