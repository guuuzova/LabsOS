#include <dlfcn.h>
#include <stdexcept>
#include <string>

#include "dynamic_library.h"

DynamicLibrary::DynamicLibrary(const std::string& path) {
    handle_ = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle_) {
        throw std::runtime_error("Cannot load library: " + std::string(dlerror()));
    }
}

DynamicLibrary::~DynamicLibrary() {
    if (handle_) {
        dlclose(handle_);
    }
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : handle_(other.handle_) {
    other.handle_ = nullptr;
}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            dlclose(handle_);
        }
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

void* DynamicLibrary::getSymbol(const char* symbolName) const {
    if (!handle_) {
        throw std::runtime_error("Library is not loaded");
    }
    void* sym = dlsym(handle_, symbolName);
    if (!sym) {
        throw std::runtime_error("Symbol '" + std::string(symbolName) + "' not found: " + dlerror());
    }
    return sym;
}