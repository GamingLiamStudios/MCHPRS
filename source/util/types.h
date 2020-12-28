#pragma once

#include <cstdint>
#include <exception>

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

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
struct u128
{
    u64 upper;
    u64 lower;
};

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
