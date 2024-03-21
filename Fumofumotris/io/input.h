#pragma once
#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "fumotris.h"

struct Axis {
    double value;
    double last_pressed;
    double last_released;
};

enum InputType {
    KEY,
    AXIS,
    JOYSTICK,
    WINDOW,
    ESCAPE
};

struct InputRecord {
    enum InputType type;
    union {
        u16 key;
        u16 axis;
        u16 joystick;
        u16 window;
    } id;

    union {
        struct {
            float value;
            bool is_down;
        } axis;
        struct {
            u64 x : 32;
            u64 y : 32;
        } joystick;
    } data;

    double timestamp;
};

const size_t io_buf_size = 8;
struct InputResult {
    struct InputRecord buf[io_buf_size];
    size_t count;
};

void StartInput(Ctrl *ctrl);