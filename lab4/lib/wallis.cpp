#include "pi_contract.h"

float CalculatePi(int N) {
    double pi = 1.0f;
    for (int i = 1; i <= N; ++i) {
        double num1 = 2.0f * i;
        double den1 = 2.0f * i - 1.0f;
        double den2 = 2.0f * i + 1.0f;
        pi *= (num1 / den1) * (num1 / den2);
    }

    return (float)(pi * 2.0);
}