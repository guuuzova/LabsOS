#include <iostream>
#include <string>

#include "proizv_contract.h"
#include "pi_contract.h"

int main() {
    std::cout << "Program 1: First derivative + Leibniz (static linking)" << std::endl;


    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "1") {
            float A, deltaX; 
            std::cin >> A >> deltaX;
            std::cout << "Derivative: " << Derivative(A, deltaX) << std::endl;
        } else if (cmd == "2") {
            int N;
            std::cin >> N;
            std::cout << "Pi: " << Pi(N) << std::endl;
        }
    }
    return 0;
}