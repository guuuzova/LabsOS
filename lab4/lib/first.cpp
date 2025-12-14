#include "proizv_contract.h"
#include <cmath>

float CalculateProizv(float A, float deltaX) {
    return (std::cos(A + deltaX) - std::cos(A)) / deltaX;
}