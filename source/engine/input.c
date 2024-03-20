#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "dictionary.h"

struct Axis
{
    uint8_t is_down;
    double last_time_down;
};

struct Controller
{
    struct Dictionary *keybinds; // u16: int
    struct Dictionary *axes; // int: InputAxis

    pthread_mutex_t mutex;
    unsigned int readers;

    size_t width;
    size_t height;
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

struct Controller NewController(uint16_t *key_codes, enum Control *axis_codes, size_t count)
{
    static struct Axis DEFAULT_INPUT_AXIS = { 0, 0 };
    static size_t DEFAULT_WIDTH = 20;
    static size_t DEFAULT_HEIGHT = 10;

    struct Dictionary *keybinds = NewDictionary(sizeof(uint16_t), sizeof(enum Control));
    struct Dictionary *axes = NewDictionary(sizeof(enum Control), sizeof(struct Axis));

    for(size_t i = 0; i < count; i++)
    {
        DictAdd(keybinds, &key_codes[i], &axis_codes[i]);
        DictAdd(axes, &axis_codes[i], &DEFAULT_INPUT_AXIS);
    }

    struct Controller controller = {
        .keybinds = keybinds, .axes = axes,

        .mutex = PTHREAD_MUTEX_INITIALIZER, .readers = 0,

        .width = DEFAULT_WIDTH, .height = DEFAULT_HEIGHT
    };

    return controller;
}

void ControllerLockWrite(struct Controller *ctrl)
{
    pthread_mutex_lock(&ctrl->mutex);
}

void ControllerUnlockWrite(struct Controller *ctrl)
{
    pthread_mutex_unlock(&ctrl->mutex);
}

void ControllerLockRead(struct Controller *ctrl)
{
    if(ctrl->readers == 0)
    {
        pthread_mutex_lock(&ctrl->mutex);
    }

    ctrl->readers += 1;
}

void ControllerUnlockRead(struct Controller *ctrl)
{
    if(ctrl->readers == 1)
    {
        pthread_mutex_unlock(&ctrl->mutex);
    }

    ctrl->readers -= 1;
}

struct Axis *ControllerKeyAxis(struct Controller *controller, uint16_t key_code)
{
    uint16_t *axis_code = DictGet(controller->keybinds, &key_code);
    if(axis_code == 0)
        return 0;

    return DictGet(controller->axes, axis_code);
}

struct Axis *ControllerCodeAxis(struct Controller *controller, enum Control axis_code)
{
    return DictGet(controller->axes, &axis_code);
}