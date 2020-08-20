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

int main(int argc, char* argv[]) {
    int width = 4;
    int height = 4;
    int depth = 0;
    int detail = 1;
    double radius = 0;
    int centerX = 0;
    int centerY = 0;
    int edge_width = -1;
    bool output_to_file = false;
    bool no_output = false;
    bool get_solution = false;
    bool blocky = false;
    std::ofstream out_file;
    unsigned int maze_seed = (unsigned int)time(NULL);
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == std::string("help")) {
            std::cout << "\nArguments:\n\n" << 
                        "-w [uint]\t\tWidth of maze\n" <<
                        "-h [uint]\t\tHeight of maze\n" <<
                        "-d [uint]\t\tDepth of maze\n" <<
                        "-s [uint]\t\tMaze seed\n" <<
                        "-detail [uint]\t\tSet maze resolution\n" <<
                        "-center [uint] [uint]\t\tStart in center. Numbers specify center size\n" << 
                        "--2d\t\t\t2D maze\n" << 
                        "--solution\t\tGenerates and outputs maze solution\n" <<
                        "--blocky\t\tBlocky corners (Only for 2D mazes atm. Also only works with radius=0)\n" <<
                        "--no_output\t\tDisables any output\n" <<
            std::endl;
            exit(EXIT_SUCCESS);
        } else if (std::string(argv[i]) == std::string("-width")) {
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
        } else if (std::string(argv[i]) == std::string("-height")) {
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
        } else if (std::string(argv[i]) == std::string("-depth")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                depth = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::string(argv[i]) == std::string("-detail")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                detail = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::string(argv[i]) == std::string("-radius")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            radius = std::stod(argv[i]);
        } else if (std::string(argv[i]) == std::string("-center")) {
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                centerX = atoi(argv[i]);
            } else {
                std::cerr << "Invalid value for \""<< argv[i-1] << "\"" << std::endl;
                exit(EXIT_FAILURE);
            }
            i++;
            if (i == argc) {
                std::cerr << "Error: No value following \""<< argv[i-1] << "\" argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (isUnsignedInteger(argv[i])) {
                centerY = atoi(argv[i]);
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
        } else if (std::string(argv[i]) == std::string("--2d")) {
            depth = 0;
        } else if (std::string(argv[i]) == std::string("--blocky")) {
            if (depth == 0) {
                blocky = true;
            }
        } else if (std::string(argv[i]) == std::string("--no_output")) {
            no_output = true;
        } else if (std::string(argv[i]) == std::string("--solution")) {
            get_solution = true;
        } else {
            std::cerr << "Unknown argument \"" << argv[i] << "\". Use \"help\" for list of valid arguments." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    Maze m = Maze(width, height, depth, 0.0, detail, radius, edge_width, centerX, centerY, maze_seed, blocky, get_solution);
    if (!no_output) {
        if (output_to_file) {
            m.outputToStream(out_file);
            out_file.close();
        } else {
            m.outputToStream(std::cout);
        }
    }
    
    return 0;
}