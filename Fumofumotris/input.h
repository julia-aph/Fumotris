#pragma once
#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const size_t IO_BUF_SIZE = 8;

enum Event {
    KEY,
    RESIZE,
    ESCAPE
};

struct KeyEvent {
    uint16_t key;
    bool is_down;
    double timestamp;
};

struct ResizeEvent {
    size_t new_x;
    size_t new_y;
};

struct InputEvent {
    enum Event type;
    union {
        struct KeyEvent key_event;
        struct ResizeEvent resize_event;
    };
};