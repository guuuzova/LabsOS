#include "bigint.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace BigInt {

BigInt::BigInt(const std::string& hex_str) {
    std::string new_str = hex_str;
    if (new_str.length() < NUM * sizeof(uint64_t) * 2) {
        new_str.insert(0, NUM * sizeof(uint64_t) * 2 - new_str.length(), '0');
    }
    for (int i = 0; i < NUM; ++i) {
        int start = new_str.length() - (i + 1) * 16;
        std::string word_hex = new_str.substr(start, 16);
        data_[i] = std::stoull(word_hex, nullptr, 16);
    }
}

BigInt& BigInt::operator+=(const BigInt& other) {
    uint64_t carry = 0;
    for (int i = 0; i < NUM; ++i) {
        uint64_t sum = data_[i] + other.data_[i];
        bool carry1 = (sum < data_[i]);
        data_[i] = sum + carry;
        bool carry2 = (data_[i] < sum);
        carry = carry1 || carry2 ? 1 : 0;
    }
    return *this;
}

BigInt BigInt::operator+(const BigInt& other) const {
    BigInt result = *this;
    result += other;
    return result;
}

void BigInt::add_word(uint64_t word) {
    if (word == 0) return;
    uint64_t sum = data_[0] + word;
    bool carry1 = (sum < data_[0]);
    data_[0] = sum;
    uint64_t carry = carry1 ? 1 : 0;
    for (int i = 1; i < NUM && carry; ++i) {
        data_[i] += carry;
        if (data_[i] == 0) {
            carry = 1;
        } else {
            carry = 0;
        }
    }
}

void BigInt::divide_by_block(uint64_t divisor) {
    if (divisor == 0) {
        throw std::runtime_error("Division by zero");
    }
    uint64_t remainder = 0;
    for (int i = NUM - 1; i >= 0; --i) {
        __uint128_t current_part = ((__uint128_t)remainder << 64) | data_[i];
        data_[i] = (uint64_t)(current_part / divisor);
        remainder = (uint64_t)(current_part % divisor);
    }
}

std::ostream& operator<<(std::ostream& os, const BigInt& big_int) {
    int first_nonzero = -1;
    for (int i = NUM - 1; i >= 0; --i) {
        if (big_int.data_[i] != 0) {
            first_nonzero = i;
            break;
        }
    }
    if (first_nonzero == -1) {
        return os << "0";
    }
    os << std::hex << big_int.data_[first_nonzero];
    for (int i = first_nonzero - 1; i >= 0; --i) {
        os << std::setw(16) << std::setfill('0') << big_int.data_[i];
    }
    os << std::dec;
    return os;
}

uint64_t BigInt::divide_by_10() {
    const uint64_t divisor = 10;
    uint64_t remainder = 0;
    for (int i = NUM - 1; i >= 0; --i) {
        __uint128_t current_part = ((__uint128_t)remainder << 64) | data_[i];
        data_[i] = (uint64_t)(current_part / divisor);
        remainder = (uint64_t)(current_part % divisor);
    }
    return remainder; 
}

std::string BigInt::to_dec() const {
    BigInt temp = *this;
    bool is_zero = true;
    for (int i = 0; i < NUM; ++i) {
        if (temp.data_[i] != 0) {
            is_zero = false;
            break;
        }
    }
    if (is_zero) {
        return "0";
    }
    std::string result;
    while (!is_zero) {
        uint64_t digit = temp.divide_by_10();
        result.push_back((char)(digit + '0')); 
        is_zero = true;
        for (int i = 0; i < NUM; ++i) {
            if (temp.data_[i] != 0) {
                is_zero = false;
                break;
            }
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}
std::mutex BigInt::BigInt::sum_mutex;
}