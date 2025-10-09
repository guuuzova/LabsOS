#include <iostream>
#include <string>
#include "os.h"

int main() {
    std::cout << "Enter filename: ";
    std::string filename;
    std::cin >> filename;

    os::PipeHandle readpipe;
    os::PipeHandle writepipe;

    if (!os::CreatePipe(readpipe, writepipe)) {
        std::cerr << "Error create pipe" << std::endl;
        return 1;
    }

    os::StartProcess psi;
    psi.path = "./child";
    psi.filename = filename;
    psi.pipe = writepipe;

    int pid = os::CreateChildProcess(psi);
    if (pid == -1) {
        std::cerr << "Error start child process" << std::endl;
        return 1;
    }

    os::ClosePipe(writepipe);
    writepipe = -1;

    char buffer[256];
    int bytes = os::ReadPipe(readpipe, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Answer: " << buffer << std::endl;
    } else {
        std::cerr << "Error";
    }

    os::ClosePipe(readpipe);
    return 0;
}