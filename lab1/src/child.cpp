#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "os.h"

float SumFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return 0.0f;
    }
    float sum = 0.0f;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float num;
        while (iss >> num) {
            sum += num;
        }
    }
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        os::Exit(1);
    }
    float answer = SumFile(argv[1]);
    std::cout << answer << std::endl;
    return 0;
}