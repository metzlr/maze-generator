#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <queue>
#include "mesh.h"


class Maze {
public:

    Maze(int numCellsX_=10, int numCellsY_=10, int numCellsZ_=1, double surface_cutoff_=0.0, int detail_=1, double radius=0.0, unsigned long seed_=(unsigned int)time(NULL), bool blocky_=false);

    unsigned int getSeed() const { return seed; }
    std::string getString() const;
    

    void outputToStream(std::ostream& stream) const;

    int getNumCellsX() const { return numCellsX; }
    int getNumCellsY() const { return numCellsY; }
    int getNumCellsZ() const { return numCellsZ; }

private:
    struct Move {
        
        inline void addIndex(VertexIndex index, bool active) {
            if (active) potential_active.push_back(index);
            fill.push_back(index);
        }
        std::vector<VertexIndex> getFill() const { return fill; }
        std::vector<VertexIndex> getPotentialActive() const { return potential_active; }
    private:
        std::vector<VertexIndex> fill;
        std::vector<VertexIndex> potential_active;
    };

    std::vector<Move> getPossibleMovesSquare(Vertex* v);
    bool generatePath(const std::vector<int>& sizes);
    std::vector<int> generatePathLengths(bool initial) const;

    Mesh* mesh;
    std::queue<Vertex*> active_vertices;

    unsigned int seed;
    int numCellsX;
    int numCellsY;
    int numCellsZ;
    double surface_cutoff;
    int scale;
    int boundary_size;
    bool flat;
};


#endif