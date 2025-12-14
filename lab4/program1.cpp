#include <iostream>
#include <string>

#include "proizv_contract.h"
#include "pi_contract.h"

int main() {
    std::cout << "Program 1: First proizv + Leibniz (static linking)" << std::endl;


    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "1") {
            float A, deltaX; 
            std::cin >> A >> deltaX;
            std::cout << "First: " << CalculateProizv(A, deltaX) << std::endl;
        } else if (cmd == "2") {
            int N;
            std::cin >> N;
            std::cout << "CalculatePi: " << CalculatePi(N) << std::endl;
        }
    }
    return 0;
}