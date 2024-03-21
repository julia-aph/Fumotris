#pragma once
#include <stdint.h>

#include "input.h"

void WindowsInit();

bool WindowsBlockInput(struct InputResult *result);

void WindowsWait(double seconds);