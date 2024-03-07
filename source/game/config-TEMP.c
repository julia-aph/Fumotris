#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "input.h"

static enum Control *CODES = {
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

static char *KEYS = {
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

static size_t CONTROL_COUNT = 9;