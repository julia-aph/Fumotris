#pragma once
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "input.h"

enum Control CODES[9] = {
    LEFT,
    RIGHT,
    SOFT_DROP,
    HARD_DROP,
    ROTATE_CCW,
    ROTATE_CW,
    ROTATE_180,
    SWAP,
    ESC
};

uint16_t KEYS[9] = {
    'a',
    'd',
    's',
    ' ',
    'q',
    'e',
    'r',
    'f',
    27
};

size_t CONTROL_COUNT = 9;