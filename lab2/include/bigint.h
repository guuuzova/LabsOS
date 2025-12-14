#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>

namespace BigInt {

constexpr int NUM = 8;

class BigInt {
private:
    uint64_t data_[NUM] = {0};

public:
    BigInt() = default;
    explicit BigInt(const std::string& hex_str);
    BigInt& operator+=(const BigInt& other);
    BigInt operator+(const BigInt& other) const;
    void add_word(uint64_t word);
    void divide_by_block(uint64_t divisor);
    uint64_t divide_by_10();
    std::string to_dec() const;
    const uint64_t* getData() const {
      return data_;
    }
    static std::mutex sum_mutex;
    friend std::ostream& operator<<(std::ostream& os, const BigInt& big_int);
};
}