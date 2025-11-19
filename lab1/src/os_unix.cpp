#include "os.h"
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>

namespace os {

int CreateChildProcess(const StartProcess& info) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error fork");
        return -1;
    }

    if (pid == 0) {
        if (info.pipe != -1) {
            if(dup2(info.pipe, STDOUT_FILENO) == -1) {
                perror("Error dup2");
                os::Exit(1);
            }
            close(info.pipe);
        }
        execl(info.path.c_str(), info.path.c_str(), info.filename.c_str(), nullptr);
        perror("Error execl");
        os::Exit(1);
    } else {
        if (info.pipe != -1) {
            close(info.pipe);
        }
        return pid;
    }
    return -1;
}

bool CreatePipe(PipeHandle& readpipe, PipeHandle& writepipe) {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        perror("Error pipe");
        return false;
    }
    readpipe  = pipefd[0];
    writepipe = pipefd[1];
    return true;
}

int ReadPipe(PipeHandle pipe, void* buf, int count) {
    if (pipe == -1 || buf == nullptr || count <= 0) {
        return -1;
    }
    int bytes = read(pipe, buf, count);
    if (bytes == -1) {
        perror("Error read");
    }
    return bytes;
}

void ClosePipe(PipeHandle pipe) {
    if (pipe != -1) {
        close(pipe);
    }
}

void Exit(int code) {
    _exit(code);
}

}