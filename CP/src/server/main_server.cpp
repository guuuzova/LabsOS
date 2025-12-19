#include "server_app.h"
#include <iostream>
#include <csignal>

ServerApp* globalAppPtr = nullptr;
void signalHandler(int signum) {
    if (globalAppPtr) {
        delete globalAppPtr;
        globalAppPtr = nullptr;
    }
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);
    try {
        ServerApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "error server: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}