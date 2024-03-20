#pragma once
#include <time.h>

#define nullptr ((void*)0)
#define null 0

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}