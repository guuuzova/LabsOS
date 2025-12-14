#include "pi_contract.h"
#include <cmath>

float CalculatePi(int N) {
    float pi = 0.0f;
    for (int i = 0; i < N; ++i) {
        float term = 1.0f / (2.0f * i + 1.0f);
        if (i % 2 == 1) {
            pi -= term;
        } else {
            pi += term;
        }
    }
    return pi * 4.0f;
}