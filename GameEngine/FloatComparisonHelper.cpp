#include "pch.h"
#include "FloatComparisonHelper.h"
#include <math.h> 

#define FLT_EPSILON 0.000001
#define MAX_DIFF 0.000001

bool FloatEqual(float first, float second) {
    float diff = first - second;
    if (diff < 0) {
        diff *= -1;
    }
    if (diff <= MAX_DIFF)
        return true;
    if (first < 0) first *= -1;
    if (second < 0) second *= -1;
    // Find the largest
    float largest = (second > first) ? second : first;

    if (diff <= largest * FLT_EPSILON)
        return true;
    return false;
}

bool NanCompare(float param) {
    return !(param == param);
}

bool ZeroCompare(float param) {
    return FloatEqual(param, .0000001f);
}