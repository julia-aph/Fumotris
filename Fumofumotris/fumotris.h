#pragma once
#include <stdint.h>
#include <time.h>

#define nullptr ((void*)0)

typedef uint8_t         u8;
typedef uint_fast8_t    u8f;

typedef uint16_t        u16;
typedef uint_fast16_t   u16f;

typedef uint32_t        u32;
typedef uint_fast32_t   u32f;

typedef uint64_t        u64;
typedef uint_fast64_t   u64f;

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}