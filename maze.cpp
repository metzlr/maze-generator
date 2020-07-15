#include "maze.h"
#include <sstream>
#include <iomanip>

/*
Vertex* Maze::Move::selectMove() {
    for (int i = 0; i < fill.size(); i++) {
        mesh->getVertex(fill[i])->unfill();
    }

}
*/

/* Maze constructor */
Maze::Maze(int w, int l, int h, double step, unsigned long seed_) {
    seed = seed_;
    srand(seed);
    width = w;
    length = l;
    height = h;
    mesh = new Mesh(w * 2 + 1, l * 2 + 1, h * 2 + 1, step);

    // Add Start/End Vertices
    mesh->getVertex(VertexIndex(1,0,0))->unfill();
    Vertex* start = mesh->getVertex(VertexIndex(1,1,0));
    start->unfill();
    active_vertices.push(start);
    Vertex* end = mesh->getVertex(VertexIndex(mesh->width()-2, mesh->length()-1, mesh->height()-1));
    end->unfill();

    //mesh->debugPrint();

    std::vector<int> initial_path_lengths = generatePathLengths(true);
    std::vector<int> path_lengths = generatePathLengths(false);

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

    //mesh->debugPrint();
}


std::vector<int> Maze::generatePathLengths(bool initial) const {
    std::vector<int> sizes;
    int area = width * length;
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


std::vector<Maze::Move> Maze::getPossibleMovesSquare(Vertex* v) {
    std::vector<Move> possible_moves;
    VertexIndex index = v->getIndex();

    Move check_index[6] ;
    for (int i = 1; i < 3; i++) {
        //check_index[0] = Move(this);
        check_index[0].addIndex(VertexIndex(index.x, index.y - i, index.z), (i == 2 ? true : false));
    }
    for (int i = 1; i < 3; i++) {
        //check_index[1] = Move(this);
        check_index[1].addIndex(VertexIndex(index.x, index.y + i, index.z), (i == 2 ? true : false));
    }
    for (int i = 1; i < 3; i++) {
        //check_index[2] = Move(this);
        check_index[2].addIndex(VertexIndex(index.x - i, index.y, index.z), (i == 2 ? true : false));
    }
    for (int i = 1; i < 3; i++) {
        //check_index[3] = Move(this);
        check_index[3].addIndex(VertexIndex(index.x + i, index.y, index.z), (i == 2 ? true : false));
    }
    for (int i = 1; i < 3; i++) {
        //check_index[4] = Move(this);
        check_index[4].addIndex(VertexIndex(index.x, index.y, index.z - i), (i == 2 ? true : false));
    }
    for (int i = 1; i < 3; i++) {
        //check_index[5] = Move(this);
        check_index[5].addIndex(VertexIndex(index.x, index.y, index.z + i), (i == 2 ? true : false));
    }

    for (int i = 0; i < 6; i++) {
        std::vector<VertexIndex> fill = check_index[i].getFill();
        bool valid = true;
        for (size_t j = 0; j < fill.size(); j++) {
            Vertex* v = mesh->getVertex(fill[j]);
            if (v == nullptr) { // Vertex is out of bounds
                valid = false;
                break;
            } 
            if (!v->isFilled()) valid = false; // Make sure vertex hasn't been unfilled yet
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
    Vertex* current;
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
        std::vector<VertexIndex> fill = selected_move.getFill();
        std::vector<VertexIndex> active = selected_move.getPotentialActive();
        for (size_t i = 0; i < fill.size(); i++) {
            mesh->getVertex(fill[i])->unfill();
        }
        // Add active vertices from move
        size_t next_move_index = rand() % active.size(); // Pick one of the active vertices as the start for next move
        for (size_t i = 0; i < active.size(); i++) {
            if (i == next_move_index) continue;
            active_vertices.push(mesh->getVertex(active[i]));
        }
        current = mesh->getVertex(active[next_move_index]);
        possible_moves = getPossibleMovesSquare(current); // Get next possible moves
        if (possible_moves.size() == 0) break; //If no possible moves, path is done
        active_vertices.push(current); // If there are moves, add vertex to active list

        len--;
    }
    //mesh->debug_print();
    return true;
}

/*
std::string Maze::getString() const {
    std::ostringstream stream;
    // Configuration
    int block_width = 3;
    char h_sym = '-';
    char v_sym = '|';
    char empty = ' ';

    // First print top row
    stream << h_sym;
    for (int w = 0; w < mesh->width(); w++) {
        Square* s = mesh->getSquare(w, 0);
        char fill = (s->getEdge(Square::EdgePosition::up)->isVisible()) ? h_sym : empty;
        stream << std::setfill(fill) << std::setw(block_width + 1) << h_sym;
    }

    // Then print middle and bottom for each node
    stream << std::endl;
    for (int h = 0; h < mesh->length(); h++) {
        for (int w = 0; w < mesh->width(); w++) {
            Square* s = mesh->getSquare(w, h);
            if (w == 0) {
                stream << ((s->getEdge(Square::EdgePosition::left)->isVisible()) ? v_sym : empty);
            }
            stream << std::setfill(empty) << std::setw(block_width+1) << ((s->getEdge(Square::EdgePosition::right)->isVisible()) ? v_sym : empty);
        }
        stream << std::endl;
        stream << h_sym;
        for (int w = 0; w < mesh->width(); w++) {
            Square* s = mesh->getSquare(w, h);
            char fill = (s->getEdge(Square::EdgePosition::down)->isVisible()) ? h_sym : empty;
            stream << std::setfill(fill) << std::setw(block_width + 1) << h_sym;
        }
        stream << std::endl;
    }
    stream << std::endl;
    return stream.str();
}
*/


void Maze::output_to_stream(std::ostream& file) const {
    file << "seed\t" << seed << std::endl;
    mesh->output_to_stream(file);
}

