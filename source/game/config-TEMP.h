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

uint8_t BLOCK_COLORS[8] = { 90, 96, 93, 95, 92, 91, 94, 33 };

size_t BLOCK_COLORS_COUNT = 8;

uint8_t I[] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0
};

uint8_t O[] = {
    2, 2,
    2, 2
};

uint8_t T[] = {
    0, 3, 0,
    3, 3, 3,
    0, 0, 0
};

uint8_t S[] = {
    0, 4, 4,
    4, 4, 0,
    0, 0, 0
};

uint8_t Z[] = {
    5, 5, 0,
    0, 5, 5,
    0, 0, 0
};

uint8_t J[] = {
    6, 0, 0,
    6, 6, 6,
    0, 0, 0
};

uint8_t L[] = {
    0, 0, 7,
    7, 7, 7,
    0, 0, 0
};