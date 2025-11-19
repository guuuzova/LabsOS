#pragma once

#include <string>
#include <cstdint>

namespace os {
using PipeHandle = intptr_t;
struct StartProcess {
    std::string path;
    std::string filename;
    PipeHandle pipe = -1;
};

int CreateChildProcess(const StartProcess& args);
bool CreatePipe(PipeHandle& readpipe, PipeHandle& writepipe);
int ReadPipe(PipeHandle pipe, void* buf, int count);
void ClosePipe(PipeHandle pipe);
void Exit(int code);
}
