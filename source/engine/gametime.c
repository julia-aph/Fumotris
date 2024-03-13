#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include "win.h"
#endif

static time_t GAME_START_SEC = 0;

void TimeInit()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    GAME_START_SEC = ts.tv_sec;
}

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec - GAME_START_SEC + (double)ts.tv_nsec / 1000000000.0;
}

double Wait(double duration)
{
    double start = GetTime();

    #ifdef _WIN32
    WindowsWait(duration);
    #endif

    double end = GetTime();
    return end - start;
}