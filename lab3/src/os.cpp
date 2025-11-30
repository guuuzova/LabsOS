#include <csignal>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "os.h"
#include "exception.h"

namespace os {

volatile sig_atomic_t signal_flag = 0;

void DefaultSignalHandler(int) {
    signal_flag = 1;
}

ProcessHandle CreateChildProcess(const std::string& exe_name, const std::string& filename, const std::string& shm_name) {
    pid_t pid = fork();
    if (pid == 0) {
        execl(exe_name.c_str(),  exe_name.c_str(), filename.c_str(), shm_name.c_str(), nullptr);
        perror("Execl failed");
        _exit(1);
    } 
    return pid;
}

void WaitChild(ProcessHandle process) {
    if (process > 0) {
        int status;
        if (waitpid(process, &status, 0) == -1) {
            throw except::ProcessExcept("Waitpid failed.");
        }
    }
}

int GetCurrentPID() {
    return getpid();
}

ProcessHandle GetParentPID() {
    return getppid();
}

SharedMemory CreateSharedMemory(const std::string& name, size_t size) {
    SharedMemory shm;
    shm.name = name;
    shm.size = size;
    shm.fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (shm.fd == -1) {
        throw except::ProcessExcept("Shm_open failed.");
    }
    if (ftruncate(shm.fd, size) == -1) {
        close(shm.fd);
        throw except::ProcessExcept("Ftruncate failed.");
    }

    shm.ptr = (char*)mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm.fd, 0);
    if (shm.ptr == MAP_FAILED) {
        close(shm.fd);
        throw except::ProcessExcept("Mmap failed.");
    }
    return shm;
}

SharedMemory Open(const std::string& name, size_t size) {
    SharedMemory shm;
    shm.name = name;
    shm.size = size;
    shm.fd = shm_open(name.c_str(), O_RDWR, 0666);
    if (shm.fd == -1) {
        throw except::ProcessExcept("Shm_open failed.");
    }
    shm.ptr = (char*)mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm.fd, 0);
    if (shm.ptr == MAP_FAILED) {
        close(shm.fd);
        throw except::ProcessExcept("Mmap failed in child.");
    }
    return shm;
}

void Unmap(SharedMemory& shm) {
    if (shm.ptr && shm.ptr != MAP_FAILED) {
        munmap(shm.ptr, shm.size);
        shm.ptr = nullptr;
    }
    if (shm.fd != -1) {
        close(shm.fd);
        shm.fd = -1;
    }
}

void Destroy(SharedMemory& shm) {
    Unmap(shm);
    shm_unlink(shm.name.c_str()); 
}

void SetSignalHandler(int signum, SignalHandler handler) {
    if (signal(signum, handler) == SIG_ERR) {
        throw except::ProcessExcept("Signal failed.");
    }
}

void SendSignal(ProcessHandle pid, int signum) {
    if (kill(pid, signum) == -1) {
        throw except::ProcessExcept("Send signal failed.");
    }
}

void WaitSignal() {
    while (!signal_flag) {
        pause();
    }
    signal_flag = 0;
}

}