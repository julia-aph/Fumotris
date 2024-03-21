#pragma once
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fumotris.h";

bool InitInputHandle();

bool InitTimer();

bool InitConsoleMode();

bool GetInput(struct InputResult *result);

bool Wait(u64 nanoseconds);