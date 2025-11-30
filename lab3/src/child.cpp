#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "os.h"
#include "exception.h"


float SumFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Child: Error opening file " << filename << std::endl;
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
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <shm_name>" << std::endl;
        _exit(1);
    }

    const std::string filename = argv[1];
    const std::string shm_name = argv[2];
    float answer = SumFile(filename);
    
    os::SharedMemory shm;
    try {
        shm = os::Open(shm_name, os::SHARED_MEMORY_SIZE);
        std::string answer_str = std::to_string(answer);
        strncpy(shm.ptr, answer_str.c_str(), shm.size - 1);
        shm.ptr[shm.size - 1] = '\0';
        os::SendSignal(os::GetParentPID(), os::CONFIRM);
        os::Unmap(shm);
    } catch (const except::ProcessExcept& e) {
        std::cerr << "Child Error: " << e.what() << std::endl;
        os::Unmap(shm);
        _exit(1);
    }
    return 0;
}