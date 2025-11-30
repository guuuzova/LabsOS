#include <iostream>
#include <string>
#include <cstdlib>
#include "os.h"
#include "exception.h"

int main() {
    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;

    const std::string shm_name = "/lab";
    os::SharedMemory shm = {nullptr, 0, -1, ""};
    os::ProcessHandle child_pid = -1;

    try {
        shm = os::CreateSharedMemory(shm_name, os::SHARED_MEMORY_SIZE);
        os::SetSignalHandler(os::CONFIRM, os::DefaultSignalHandler);
        child_pid = os::CreateChildProcess("./child", filename, shm_name);
        os::WaitSignal();
        std::string answer_str(shm.ptr);
        std::cout << "Answer: " << answer_str << std::endl;
    } catch (const except::ProcessExcept& e) {
        std::cerr << "Parent Error: " << e.what() << std::endl;
    }

    if (child_pid > 0) {
        try {
            os::WaitChild(child_pid);
        } catch (const except::ProcessExcept& e) {
            std::cerr << "Parent Error : " << e.what() << std::endl;
        }
    }
    if (shm.ptr) {
        os::Destroy(shm);
    }
    return 0;
}