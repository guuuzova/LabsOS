#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <memory>
#include <stdexcept>

#include "dynamic_library.h" 
#include "proizv_contract.h" 
#include "pi_contract.h" 

static constexpr auto proizv_symbol = "Derivative";
static constexpr auto pi_symbol = "Pi";

static constexpr auto LIB1_PROIZV = "lib/liblibrary_first.so"; 
static constexpr auto LIB1_PI = "lib/liblibrary_leibniz.so";
static constexpr auto LIB2_PROIZV = "lib/liblibrary_second.so";
static constexpr auto LIB2_PI = "lib/liblibrary_wallis.so";


using DerivativeFunc = float (*)(float, float);
using PiFunc = float (*)(int);

std::unique_ptr<DynamicLibrary> proizv_lib;
std::unique_ptr<DynamicLibrary> pi_lib;
DerivativeFunc proizv_func = nullptr;
PiFunc pi_func = nullptr;

void load_version(const std::string& proizv_path, const std::string& pi_path) {
    proizv_lib = std::make_unique<DynamicLibrary>(proizv_path);
    pi_lib = std::make_unique<DynamicLibrary>(pi_path);
    proizv_func = proizv_lib->getSymbolAs<DerivativeFunc>(proizv_symbol);
    pi_func = pi_lib->getSymbolAs<PiFunc>(pi_symbol);
}

int main() {
    std::string current_proizv_path = LIB1_PROIZV;
    std::string current_pi_path = LIB1_PI;
    load_version(current_proizv_path, current_pi_path);
    std::cout << "Initial version first or Leibniz" << std::endl;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;
        if (cmd == "0") {
            try {
                if (current_proizv_path == LIB1_PROIZV) {
                    current_proizv_path = LIB2_PROIZV;
                    current_pi_path = LIB2_PI;
                    load_version(current_proizv_path, current_pi_path);
                    std::cout << "--> Switched to second or Wallis" << std::endl;
                } else {
                    current_proizv_path = LIB1_PROIZV;
                    current_pi_path = LIB1_PI;
                    load_version(current_proizv_path, current_pi_path);
                    std::cout << "--> Switched to first or Leibniz" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Switch failed: " << e.what() << std::endl;
            }
        }
        else if (cmd == "1") {
            float A, deltaX;
            if (ss >> A >> deltaX) {
                float result = proizv_func(A, deltaX);
                std::cout << "Derivative: " << result << std::endl;
            } else {
                 std::cout << "Invalid arguments";
            }
        }
        else if (cmd == "2") {
            int N;
            if (ss >> N) {
                float pi_result = pi_func(N);
                std::cout << "Pi: " << pi_result << std::endl;
            } else {
                 std::cout << "Invalid arguments" << std::endl;
            }
        }
        else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }
    return 0;
}