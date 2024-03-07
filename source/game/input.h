#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "dictionary.h"

struct InputAxis
{
    uint8_t is_down;
    double last_time_down;
};

struct Controller
{
    struct Dictionary *keybinds; // char: u16
    struct Dictionary *axes; // u16: inputAxis*

    pthread_mutex_t mutex;
};

enum Controls
{
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

struct Controller NewController(char *keys, int *codes, int count);

struct InputAxis *ControllerGetAxis(struct Controller *controller, char key);

struct InputAxis *ControllerAxis(struct Controller *controller, uint16_t code);

double GetTime();

