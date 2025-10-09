#include "os.h"
#include <windows.h>
#include <string>

namespace os {

int CreateChildProcess(const StartProcess& args) {
    std::string cmd = args.path + " " + args.filename;

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    if (args.pipe != -1) {
        si.hStdOutput = static_cast<HANDLE>(args.pipe);
        si.hStdError = si.hStdOutput;
    } else {
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }
    BOOL success = CreateProcessA(nullptr, const_cast<char*>(cmd.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi
    );

    if (!success) {
        return -1;
    }
    int pid = static_cast<int>(pi.dwProcessId);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return pid;
}

bool CreatePipe(PipeHandle& readpipe, PipeHandle& writepipe) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    HANDLE hRead, hWrite;

    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return false;
    }
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    readpipe = static_cast<PipeHandle>(static_cast<intptr_t>(hRead));
    writepipe = static_cast<PipeHandle>(static_cast<intptr_t>(hWrite));
    return true;
}

int ReadPipe(PipeHandle pipe, void* buf, int count) {
    if (pipe == -1) {
        return -1;
    }
    HANDLE h = static_cast<HANDLE>(pipe);
    DWORD bytesRead = 0;

    if (!ReadFile(h, buf, static_cast<DWORD>(count), &bytesRead, nullptr)) {
        return -1;
    }

    return static_cast<int>(bytesRead);
}

void ClosePipe(PipeHandle pipe) {
    if (pipe != -1) {
        HANDLE h = static_cast<HANDLE>(pipe);
        CloseHandle(h);
    }
}

void Exit(int code) {
    ExitProcess(static_cast<UINT>(code));
}

}