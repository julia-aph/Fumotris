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
#include "winio.h"
#endif

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

bool dispatch(Ctrl *ctrl, struct InputRecord *record)
{
    switch (record->type) {
    case KEY:
        return CtrlUpdateKey(ctrl, record);
    case AXIS:
        return CtrlUpdateAxis(ctrl, record);
    case JOYSTICK:
        return CtrlUpdateJoystick(ctrl, record);
    case WINDOW:
        return CtrlUpdateWindow(ctrl, record);
    case ESCAPE:
        exit(1);
    }
    return false;
}

void block_input(void *args)
{
    Ctrl *ctrl = args;
    struct InputResult result;

input_loop:
    bool success;
    #ifdef _WIN32
    success = WindowsBlockInput(&result);
    #endif

    if (!success)
        exit(1);

    double now = GetTime();
    for (size_t i = 0; i < result.count; i++) {
        result.buf[i].timestamp = now;
        bool success = dispatch(ctrl, &result.buf[i]);
    }

    goto input_loop;
}

void StartInput(Ctrl *ctrl)
{
    pthread_t input_thread;
    pthread_create(&input_thread, nullptr, block_input, ctrl);
}