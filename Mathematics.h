#pragma once
#include "Lab3.h"

void FillCosXAndSinX(std::vector<double>& cos, std::vector<double>& sin, double start, double end)
{
    const int numPoints = 100;

    calculateSinArray(start, end, numPoints, sin);
    calculateCosArray(start, end, numPoints, cos);
}