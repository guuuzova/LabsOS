#include "pi_contract.h"
#include <cmath>

float Pi(int K) {
    float pi = 0.0f;
    for (int i = 0; i < K; ++i) {
        float term = 1.0f / (2.0f * i + 1.0f);
        if (i % 2 == 1) {
            pi -= term;
        } else {
            pi += term;
        }
    }
    return pi * 4.0f;
}