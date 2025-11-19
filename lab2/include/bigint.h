#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>

struct BigInt {
  private:
    uint64_t parts[8];
  public:
    BigInt();
    BigInt(const BigInt& other);
    BigInt(BigInt&& other) noexcept;
    BigInt& operator=(const BigInt& other) = default;
    BigInt& operator=(BigInt&& other) noexcept;
    ~BigInt() = default;

    void Add(const BigInt& other);
    uint64_t Divide(uint64_t divisor, BigInt& quotient) const;
    void Round();
    int ReadFromHex(const char* hex, size_t len);
    void Print() const;
};