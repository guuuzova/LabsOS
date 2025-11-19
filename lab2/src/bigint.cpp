#include "bigint.h"


BigInt::BigInt() {
    for (int i = 0; i < 8; i++) {
        parts[i] = 0;
    }
}

BigInt::BigInt(const BigInt& other) {
    for (int i = 0; i < 8; i++) {
        parts[i] = other.parts[i];
    }
}

BigInt::BigInt(BigInt&& other) noexcept {
    for (int i = 0; i < 8; i++) {
        parts[i] = other.parts[i];
        other.parts[i] = 0;
    }
}


BigInt& BigInt::operator=(BigInt&& other) noexcept {
    if (this != &other) {
        for (int i = 0; i < 8; i++) {
            parts[i] = other.parts[i];
            other.parts[i] = 0;
        }
    }
    return *this;
}

void BigInt::Add(const BigInt& other) {
    uint64_t carry = 0;
    for (int i = 7; i >= 0; --i) {
        uint64_t a = parts[i];
        uint64_t b = other.parts[i];
        uint64_t sum_ab = a + b;
        uint64_t carry1 = (sum_ab < a);
        uint64_t sum = sum_ab + carry;
        uint64_t carry2 = (sum < carry);        
        carry = carry1 | carry2;
        parts[i] = sum;
    }
}

int BigInt::ReadFromHex(const char* hex, size_t len) {
    for (int i = 0; i < 8; ++i) {
        parts[i] = 0;
    }
    int word_idx = 7;
    int shift = 0;
    for (size_t i = len; i > 0; --i) {
        char c = hex[i - 1];
        unsigned digit;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else {
            return -1;
        }
        parts[word_idx] |= static_cast<uint64_t>(digit) << shift;
        shift += 4;
        if (shift == 64) {
            shift = 0;
            --word_idx;
            if (word_idx < 0) {
                break; 
            }
        }
    }
    return 0;
}

uint64_t BigInt::Divide(uint64_t divisor, BigInt& quotient) const {

    __uint128_t remainder = 0;
    for (int i = 0; i < 8; ++i) {
        __uint128_t temp = (remainder << 64) | parts[i];
        quotient.parts[i] = static_cast<uint64_t>(temp / divisor);
        remainder = temp % divisor;
    }
    return static_cast<uint64_t>(remainder);
}

void BigInt::Round() {
        uint64_t carry = 1;
    for (int i = 7; i >= 0 && carry; --i) {
        uint64_t old = parts[i];
        parts[i] = old + carry;
        carry = (parts[i] < old);
    }
}

void BigInt::Print() const {
    for (int i = 0; i < 8; ++i) {
        std::cout << std::hex << std::setw(16) << std::setfill('0') << parts[i];
    }
    std::cout << std::dec;
}
