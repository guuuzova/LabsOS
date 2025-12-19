#include "client_app.h"
#include <iostream>


int main() {
    std::cout << "Player NAVAL BATTLE" << std::endl;
    try {
        ClientApp app;
        app.start();
    } catch (const std::exception& e) {
        std::cerr << "error player: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}