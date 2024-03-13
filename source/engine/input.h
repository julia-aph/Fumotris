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
    struct Dictionary *keybinds; // u16: int
    struct Dictionary *axes; // int: InputAxis

    pthread_mutex_t mutex;
};

enum Control
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

struct Controller NewController(uint16_t *key_codes, enum Control *axis_codes, size_t count);

struct InputAxis *ControllerKeyAxis(struct Controller *controller, uint16_t key_code);

struct InputAxis *ControllerCodeAxis(struct Controller *controller, enum Control axis_code);

void StartInputThread(struct Controller *ctrl);

void ControllerLockWrite(struct Controller *ctrl);

void ControllerUnlockWrite(struct Controller *ctrl);

void ControllerLockRead(struct Controller *ctrl);

void ControllerUnlockRead(struct Controller *ctrl);