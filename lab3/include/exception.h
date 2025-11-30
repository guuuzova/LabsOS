#pragma once

#include <stdexcept>
#include <string>

namespace except {
class ProcessExcept : public std::runtime_error {
public:
    explicit ProcessExcept(const std::string& message) 
        : std::runtime_error(message) {}
};

}