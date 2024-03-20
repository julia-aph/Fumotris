#pragma once
#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"

struct Axis {
    double value;
    double last_pressed;
    double last_released;
};

typedef uint16_t key_type;
typedef uint8_t mouse_type;

enum InputType {
    KEY,
    MOUSE_BUTTON,

    MOUSE_MOVE,
    WINDOW,
    ESCAPE
};

struct InputUpdate {
    enum InputType type;
    union {
        key_type key;
        mouse_type button;
    } ident;

    union {
        double value;
        struct {
            uint64_t x : 32;
            uint64_t y : 32;
        };
    } data;

    double timestamp;
    bool is_down;
};

const size_t IO_BUF_SIZE = 8;

void StartInput(Ctrl *ctrl);