#pragma once
#include <stdint.h>

#include "input.h"

void WindowsInit();

bool WindowsBlockInput(struct InputEvent event_buf[IO_BUF_SIZE]);

void WindowsWait(double seconds);