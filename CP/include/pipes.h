#pragma once

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class NamedPipe {
public:
  std::string path;
  int fd;

  NamedPipe(std::string _path) : path(_path), fd(-1) {}

  bool CreatePipe() {
    if (mkfifo(path.c_str(), 0666) == -1) {
      if (errno != EEXIST) {
        return false;
      }
    }
    return true;
  }

  bool OpenPipe(int mode) {
    fd = open(path.c_str(), mode);
    return (fd != -1);
  }

  void ClosePipe() {
    if (fd != -1) {
      close(fd);
      fd = -1;
    }
  }

  bool Send(const void *buffer, size_t size) {
    if (fd == -1) {
      return false;
    }
    return write(fd, buffer, size) == (ssize_t)size;
  }

  bool Receive(void *buffer, size_t size) {
    if (fd == -1) {
      return false;
    }
    return read(fd, buffer, size) == (ssize_t)size;
  }
  ~NamedPipe() {
    ClosePipe();
    }
};