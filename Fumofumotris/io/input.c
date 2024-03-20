#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fumotris.h"
#include "controller.h"

#ifdef _WIN32
#include "win.h"
#endif

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
    };

    union {
        double value;
        struct {
            uint64_t x : 32;
            uint64_t y : 32;
        };
    };

    double timestamp;
    bool is_down;
};

bool dispatch(Ctrl *ctrl, struct InputUpdate *event)
{
    switch (event->type) {
    case KEY:
        return CtrlUpdateKey(ctrl, event);
    case MOUSE_BUTTON:
        return CtrlUpdateBut(ctrl, event);
    
    case MOUSE_MOVE:
        return CtrlUpdateMouse(ctrl, event);
    case WINDOW:
        return CtrlUpdateWin(ctrl, event);
    case ESCAPE:
        exit(1);
    }
    return false;
}

const size_t IO_BUF_SIZE = 8;
void block_input(void *args)
{
    Ctrl *ctrl = args;

    struct InputUpdate buf[IO_BUF_SIZE];
    size_t count;

input_loop:
    bool success;
    #ifdef _WIN32
    success = WindowsBlockInput(buf, &count);
    #endif
    if (!success)
        exit(1);

    for (size_t i = 0; i < count; i++) {
        bool success = dispatch(ctrl, buf + i);
    }

    goto input_loop;
}

void StartInput(Ctrl *ctrl)
{
    pthread_t input_thread;
    pthread_create(&input_thread, null, block_input, ctrl);
}