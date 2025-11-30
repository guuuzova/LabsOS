#pragma once

#include <csignal>
#include <string>
#include <sys/types.h>

namespace os {

using ProcessHandle = pid_t;
using FileHandle = int;
using SignalHandler = void (*)(int);

constexpr int CONFIRM = SIGUSR2;

constexpr size_t SHARED_MEMORY_SIZE = 1024;

struct SharedMemory {
    char* ptr;
    size_t size;
    FileHandle fd;
    std::string name;
};

ProcessHandle CreateChildProcess(const std::string& exe_name, const std::string& filename, const std::string& shm_name);

void WaitChild(ProcessHandle process);

int GetCurrentPID();

ProcessHandle GetParentPID();

SharedMemory CreateSharedMemory(const std::string& name, size_t size);

SharedMemory Open(const std::string& name, size_t size);

void Destroy(SharedMemory& shm);

void Unmap(SharedMemory& shm);

void SetSignalHandler(int signum, SignalHandler handler);

void DefaultSignalHandler(int signum);

void SendSignal(ProcessHandle pid, int signum);

void WaitSignal();

}