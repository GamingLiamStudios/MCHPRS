#pragma once

#include <cstdint>

struct u128
{
    uint64_t upper;
    uint64_t lower;
};

struct f64x3
{
    double x;
    double y;
    double z;
};

struct f32x2
{
    float x;
    float y;
};
