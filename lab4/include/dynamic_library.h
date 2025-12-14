#pragma once

#include <string>
#include <memory>
#include <stdexcept>

class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::string& path);
    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    void* getSymbol(const char* symbolName) const;

    template<typename T>
    T getSymbolAs(const char* symbolName) const {
        return reinterpret_cast<T>(getSymbol(symbolName));
    }

private:
    void* handle_ = nullptr;
};