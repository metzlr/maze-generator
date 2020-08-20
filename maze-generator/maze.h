#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <queue>
#include "mesh.h"


class Maze {
public:

    Maze(int num_cellsX_=10, int num_cellsY_=10, int num_cellsZ_=1, double surface_cutoff_=0.0, int detail_=1, double path_radius=0.0, int edge_width=-1, int centerX=0, int centerY=0, unsigned long seed_=(unsigned int)time(NULL), bool blocky_=false, bool find_solution=false);
    ~Maze();

    unsigned int getSeed() const { return seed; }
    std::string getString() const;
    

    void outputToStream(std::ostream& stream) const;

    int getNumCellsX() const { return num_cellsX; }
    int getNumCellsY() const { return num_cellsY; }
    int getNumCellsZ() const { return num_cellsZ; }

private:
    struct Move {
        
        inline void addIndex(VoxelIndex index, bool active) {
            if (active) potential_active.push_back(index);
            fill.push_back(index);
        }
        std::vector<VoxelIndex> getFill() const { return fill; }
        std::vector<VoxelIndex> getPotentialActive() const { return potential_active; }
    private:
        std::vector<VoxelIndex> fill;
        std::vector<VoxelIndex> potential_active;
    };

    std::vector<Move> getPossibleMovesSquare(Voxel* v);
    bool generatePath(const std::vector<int>& sizes);
    std::vector<int> generatePathLengths(bool initial) const;
    std::vector<Voxel*> findSolution(Voxel* start, Voxel* end) const;

    std::pair<Voxel*, Voxel*> generateEntrances1(int z);
    std::pair<Voxel*, Voxel*> generateEntrances2(int z, int center_size_x, int center_size_y);

    Mesh* mesh;
    std::queue<Voxel*> active_vertices;
    std::vector<Voxel*> solution;
    Voxel* start;
    Voxel* end;
    unsigned int seed;
    int num_cellsX;
    int num_cellsY;
    int num_cellsZ;
    double surface_cutoff;
    int scale;
    int boundary_size;
    bool flat;
};


#endif