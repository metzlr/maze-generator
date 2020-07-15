#include <iostream>
#include "maze.h"

bool isUnsignedInteger(const char * str) {
    int i = 0;
    while (str[i] != '\0') {
        if (!isdigit(str[i])) return false;
        i++;
    }
    return true;
}

//int main(int argc, char* argv[]) {
int main() {
    /*
    int width = 10;
    int height = 10;
    bool output_to_file = false;
    bool output_data = false;
    std::ofstream out_file;
    unsigned int maze_seed = (unsigned int)time(NULL);
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == std::string("help")) {
            std::cout << "\nArguments:\n\n-w [uint]   Width of maze\n-h [uint]   Height of maze\n-s [uint]   Maze seed\n--entrance_offset   Enables random offsets for entrances. Not recommended.\n" << std::endl;
            exit(EXIT_SUCCESS);
        } else if (std::string(argv[i]) == std::string("-w")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \"" << argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                width = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::string(argv[i]) == std::string("-h")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                height = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::string(argv[i]) == std::string("-s")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << std::string(argv[i]) << std::endl;
            if (isUnsignedInteger(argv[i])) {
                maze_seed = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::string(argv[i]) == std::string("-f")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            out_file.open(argv[i], std::ios::out | std::ios::trunc);
            if (out_file.good()) {
                output_to_file = true;
            } else {
                std::cerr << "Error opening output file" << std::endl;
            }
        } else if (std::string(argv[i]) == std::string("-o")) {
            output_data = true;
        } else {
            std::cerr << "Unknown argument \"" << argv[i] << "\". Use \"help\" for list of valid arguments." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    Maze m = Maze(width, height, maze_seed);
    if (output_to_file) {
        m.output_to_stream(out_file);
        out_file.close();
    }
    if (output_data) {
        m.output_to_stream(std::cout);
    } else {
        std::cout << m.getString() << std::endl;
    }
    */
    Maze m = Maze(3, 3, 0, 1.0);
    m.output_to_stream(std::cout);
    return 0;
}