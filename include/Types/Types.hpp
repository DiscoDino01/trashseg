#pragma once

#include<stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;
typedef long double f128;

/*enum d_val {lo = 0x0, hi = 0x1};
template<typename T>
operator d_val(T &val) {
    return (!!val) ? hi : lo;
};*/

// DIGITAL VALUE OF DIGITAL PINS
typedef i32 d_val;

// ANALOG VALUE OF ANALOG PINS
typedef i32 a_val;

//typedef void none