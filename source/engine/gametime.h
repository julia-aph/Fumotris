#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include "win.h"
#endif

void TimeInit();

double GetTime();

double Wait(double duration);