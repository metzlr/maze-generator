#include "maze.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cassert>


/* Maze constructor */
Maze::Maze(int num_cellsX_, int num_cellsY_, int num_cellsZ_, double surface_cutoff_, int detail_, double path_radius, int edge_width_, unsigned long seed_, bool blocky_, bool find_solution) {
    seed = seed_;
    srand(seed);
    num_cellsX = num_cellsX_;
    num_cellsY = num_cellsY_;
    num_cellsZ = num_cellsZ_;
    surface_cutoff = surface_cutoff_;
    // Setup maze constants
    if (num_cellsZ == 0)
        flat = true;
    else
        flat = false;
    if (detail_ < 1) detail_ = 1;
    scale = std::pow(2, detail_);
    if (edge_width_ < 0) edge_width_ = (scale/2 == 1 ? 1 : scale/2);
    boundary_size = scale/2 + edge_width_;
    // Generate mesh
    if (flat) {     // 2D mesh
        mesh = new Mesh(num_cellsX * scale + 1 + edge_width_*2, num_cellsY * scale + 1 + edge_width_*2, 1);
    } else {        // 3D mesh
        mesh = new Mesh(num_cellsX * scale + 1 + edge_width_*2, num_cellsY * scale + 1 + edge_width_*2, num_cellsZ * scale + 1 + edge_width_);
    }
    int zBoundary = 0;
    if (!flat) {
        zBoundary = boundary_size;
    }
    for (int i = 0; i < boundary_size; i++) {   // Create start opening
        
        VoxelIndex index = VoxelIndex(boundary_size, i, zBoundary);
        mesh->getVoxel(index)->setValue(1.0);
    }
    for (int i = 0; i < boundary_size; i++) {   // Create end opening
        VoxelIndex index = VoxelIndex(mesh->getNumVoxelsX()-1 - boundary_size, mesh->getNumVoxelsY()-1 - i, zBoundary);
        mesh->getVoxel(index)->setValue(1.0);
    }
    // Get start and end vertex
    Voxel* start = mesh->getVoxel(VoxelIndex(boundary_size, boundary_size, zBoundary));
    Voxel* end = mesh->getVoxel(VoxelIndex(mesh->getNumVoxelsX()-1 - boundary_size, mesh->getNumVoxelsY()-1 - boundary_size, zBoundary));
    
    start->setValue(1.0);
    active_vertices.push(start);    // Add it to active vertices
    //Generate path lengths. Different possible path lengths for initial path and subsequent paths
    std::vector<int> initial_path_lengths = generatePathLengths(true);
    std::vector<int> path_lengths = generatePathLengths(false);

    // Loop that fills in the maze
    bool not_done = true;
    int path_count = 0;
    while (not_done) {
        if (path_count == 0) {
            not_done = generatePath(initial_path_lengths);
        } else {
            not_done = generatePath(path_lengths);
        }
        if (not_done) path_count++;
    }
    if (find_solution) {
        solution = findSolution(start, end);
    }
    // Edit finished maze
    mesh->blend(path_radius, zBoundary, 0.5);   // Blend paths
    mesh->cutoff(flat);  // Empty outermost voxels on all sides
    // Create triangles
    if (flat) {
        mesh->triangulate2D(surface_cutoff, blocky_);  
    } else {
        mesh->depthFill(zBoundary);     // Extend the maze vertically so its 3D
        mesh->triangulate3D(surface_cutoff);
    }
}

Maze::~Maze() {
    delete mesh;
}

struct PathNode {
    PathNode(Voxel* voxel_, PathNode* prev_) : voxel(voxel_), prev(prev_) {}
    Voxel* voxel;
    PathNode* prev;
};

std::vector<Voxel*> Maze::findSolution(Voxel* start, Voxel* end) const {
    std::vector<Voxel*> final_path;
    std::vector<PathNode*> all_nodes;
    std::vector<PathNode*> current_nodes;
    std::vector<PathNode*> next_nodes;
    all_nodes.push_back(new PathNode(start, nullptr));
    current_nodes.push_back(all_nodes[0]);
    bool done = false;
    while (!done) {
        for (uint i = 0; i < current_nodes.size(); i++) {
            std::vector<Voxel*> check;
            VoxelIndex index = current_nodes[i]->voxel->getIndex();
            // Get surrounding nodes to find valid moves
            check.push_back(mesh->getVoxel(VoxelIndex(index.x, index.y + scale, index.z)));
            check.push_back(mesh->getVoxel(VoxelIndex(index.x, index.y - scale, index.z)));
            check.push_back(mesh->getVoxel(VoxelIndex(index.x + scale, index.y, index.z)));
            check.push_back(mesh->getVoxel(VoxelIndex(index.x - scale, index.y, index.z)));
            for (uint j = 0; j < check.size(); j++) {
                // Make sure move exists
                if (check[j] == nullptr || check[j]->getValue() <= surface_cutoff) continue;
                // Make sure move is not previous node
                if (current_nodes[i]->prev != nullptr) {
                    if (current_nodes[i]->prev->voxel->getID() == check[j]->getID()) continue;
                }
                bool valid = false;
                // Check if there is a valid path from current node to move
                if (j == 0 && mesh->getVoxel(VoxelIndex(index.x, index.y + 1, index.z))->getValue() > surface_cutoff) {       // UP
                    valid = true;
                } else if (j == 1 && mesh->getVoxel(VoxelIndex(index.x, index.y - 1, index.z))->getValue() > surface_cutoff) {  // DOWN
                    valid = true;
                } else if (j == 2 && mesh->getVoxel(VoxelIndex(index.x + 1, index.y, index.z))->getValue() > surface_cutoff) {  // RIGHT
                    valid = true;
                } else if (j == 3 && mesh->getVoxel(VoxelIndex(index.x - 1, index.y, index.z))->getValue() > surface_cutoff) {  // LEFT
                    valid = true;
                }
                if (valid == true) {    // Move is valid
                    if (check[j]->getID() == end->getID()) {  // Check if reached end of maze
                        VoxelIndex end_index = end->getIndex();
                        final_path.push_back(mesh->getVoxel(VoxelIndex(end_index.x, end_index.y + boundary_size, end_index.z)));
                        // Trace back steps to create path
                        final_path.push_back(end);
                        PathNode* node = current_nodes[i];
                        while (node != nullptr) {
                            final_path.push_back(node->voxel);
                            node = node->prev;
                        }
                        VoxelIndex start_index = start->getIndex();
                        final_path.push_back(mesh->getVoxel(VoxelIndex(start_index.x, start_index.y - boundary_size, start_index.z)));
                        done = true;
                        break;
                    } else {    // Not at end, add move to list of next moves
                        next_nodes.push_back(new PathNode(check[j], current_nodes[i]));
                    }
                }
            }
            if (done) break;
        }
        // Add new moves to list of all moves
        for (uint i = 0; i < next_nodes.size(); i++) {
            all_nodes.push_back(next_nodes[i]);
        }
        if (!done) {    // If not done, replace current moves with next moves and clear next moves list
            current_nodes = next_nodes;
            next_nodes.clear();
        } else {    // If done, deallocate memory
            for (uint i = 0; i < all_nodes.size(); i++) {
                delete all_nodes[i];
            }
        }
    }
    return final_path;
}


std::vector<int> Maze::generatePathLengths(bool initial) const {
    std::vector<int> sizes;
    int area = num_cellsX * num_cellsY;
    if (initial) {
        for (int i = 0; i < 10; i++) {
            sizes.push_back(area*0.5);
        }
    } else {
        for (int i = 0; i < 10; i++) {
            if (i < 3) {
                sizes.push_back(area*0.1);
            } else if (i < 8) {
                sizes.push_back(area*0.2);
            } else {
                sizes.push_back(area*0.4);
            }
        }
    }
    return sizes;
}


std::vector<Maze::Move> Maze::getPossibleMovesSquare(Voxel* v) {
    std::vector<Move> possible_moves;
    VoxelIndex index = v->getIndex();

    Move check_index[4];
    for (int i = 1; i <= scale; i++) {
        check_index[0].addIndex(VoxelIndex(index.x, index.y - i, index.z), (i == scale ? true : false));
    }
    for (int i = 1; i <= scale; i++) {
        check_index[1].addIndex(VoxelIndex(index.x, index.y + i, index.z), (i == scale ? true : false));
    }
    for (int i = 1; i <= scale; i++) {
        check_index[2].addIndex(VoxelIndex(index.x - i, index.y, index.z), (i == scale ? true : false));
    }
    for (int i = 1; i <= scale; i++) {
        check_index[3].addIndex(VoxelIndex(index.x + i, index.y, index.z), (i == scale ? true : false));
    }

    for (int i = 0; i < 4; i++) {
        std::vector<VoxelIndex> fill = check_index[i].getFill();
        bool valid = true;
        for (size_t j = 0; j < fill.size(); j++) {
            Voxel* v = mesh->getVoxel(fill[j]);
            if (v == nullptr) { // Vertex is out of bounds
                valid = false;
                break;
            } else if (v->getIndex().x < boundary_size || v->getIndex().x > mesh->getNumVoxelsX() - 1 - boundary_size || v->getIndex().y < boundary_size || v->getIndex().y > mesh->getNumVoxelsY() - 1 - boundary_size) {
                // Vertex is on an edge
                valid = false;
            }
            if (v->getValue() > surface_cutoff) valid = false; // Make sure vertex hasn't been unfilled yet
        }
        if (valid) possible_moves.push_back(check_index[i]);
    }
    return possible_moves;
}

// Attempts to generate maze by adding paths of pre-determined length. Works much better than generate_path1
bool Maze::generatePath(const std::vector<int>& sizes) {
    if (active_vertices.size() == 0) { // No more open spots
        return false;
    }
    // Randomly select path length
    int len = sizes[rand() % sizes.size()];

    std::vector<Move> possible_moves;

    // Select active vertex
    Voxel* current;
    while (true) {
        if (active_vertices.size() == 0) return false; //If no active vertices, maze is done
        current = active_vertices.front();
        // Check for valid moves
        possible_moves = getPossibleMovesSquare(current);
        if (possible_moves.size() > 0) break; // Has valid moves
        active_vertices.pop(); // If no possible moves, remove vertex from active vertices
    }

    while (len > 0) {
        // Pick random move from possible moves
        Move selected_move = possible_moves[rand() % possible_moves.size()];
        std::vector<VoxelIndex> fill = selected_move.getFill();
        std::vector<VoxelIndex> active = selected_move.getPotentialActive();
        for (size_t i = 0; i < fill.size(); i++) {
            mesh->getVoxel(fill[i])->setValue(1.0);
        }
        // Add active vertices from move
        size_t next_move_index = rand() % active.size(); // Pick one of the active vertices as the start for next move
        for (size_t i = 0; i < active.size(); i++) {
            if (i == next_move_index) continue;
            active_vertices.push(mesh->getVoxel(active[i]));
        }
        current = mesh->getVoxel(active[next_move_index]);
        possible_moves = getPossibleMovesSquare(current); // Get next possible moves
        if (possible_moves.size() == 0) break; //If no possible moves, path is done
        active_vertices.push(current); // If there are moves, add vertex to active list

        len--;
    }
    return true;
}


void Maze::outputToStream(std::ostream& stream) const {
    stream << "seed\t" << seed << std::endl;
    if (flat) {     // 2D mesh, dont include vertex normals
        mesh->outputTriangleVertices(stream, false);
    } else {        // 3D mesh, include normals
        mesh->outputTriangleVertices(stream, true);
    }
    mesh->outputTriangles(stream);
    if (solution.size() > 0) {
        for (uint i = 0; i < solution.size(); i++) {
            VoxelIndex index = solution[i]->getIndex();
            std::cout << "path\t" << index.x << "\t" << index.y << "\t" << index.z << "\t" << std::endl;
        }
    }
}

