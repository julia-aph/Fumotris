#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "controller.h"
#include "win.h"

bool WindowsInit()
{
    if (!InitInputHandle())
        return false;

    if (!InitTimer())
        return false;

    if(!InitConsoleMode())
        return false;

    return true;
}

bool WindowsBlockInput(struct InputResult *result)
{
    return GetInput(result);
}

bool WindowsWait(double seconds)
{
    return Wait(seconds / 1000000000.0);
}