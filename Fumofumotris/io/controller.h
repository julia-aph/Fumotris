#pragma once
#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fumotris.h"
#include "input.h"

struct Axis {
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

    double last_pressed;
    double last_released;
};

enum KeyCode {
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

enum AxisCode {
    VSCROLL,
    HSCROLL
};

enum JoystickCode {
    MOUSE
};

struct Ctrl {
    struct ctrl_dict *dict;
};
typedef struct Ctrl Ctrl;

struct Ctrl NewCtrl();

bool CtrlUpdateKey(Ctrl *ctrl, struct InputRecord *record);

bool CtrlUpdateAxis(Ctrl *ctrl, struct InputRecord *record);

bool CtrlUpdateJoystick(Ctrl *ctrl, struct InputRecord *record);

bool CtrlUpdateWindow(Ctrl *ctrl, struct InputRecord *record);

struct Axis *CtrlGetKey(Ctrl *ctrl, enum KeyCode key);

struct Axis *CtrlGetAxis(Ctrl *ctrl, enum AxisCode axis);

struct Axis *CtrlGetJoystick(Ctrl *ctrl, enum JoystickCode joystick);

struct Axis *CtrlGetWindow(Ctrl *ctrl, u16f window);