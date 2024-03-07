#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

#include "input.h"

void WindowsInit();

void WindowsBlockInput(struct Controller *controller);

void WindowsWait(double seconds);