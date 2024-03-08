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

struct Controller NewController(uint16_t *key_codes, enum Control *axis_codes, size_t count)
{
    static struct InputAxis default_input_axis = { 0, 0 };

    struct Dictionary *keybinds = NewDictionary(sizeof(uint16_t), sizeof(enum Control));
    struct Dictionary *axes = NewDictionary(sizeof(enum Control), sizeof(struct InputAxis));

    for(size_t i = 0; i < count; i++)
    {
        DictAdd(keybinds, &key_codes[i], &axis_codes[i]);
        DictAdd(axes, &axis_codes[i], &default_input_axis);
    }

    struct Controller controller = {
        .keybinds = keybinds, .axes = axes,

        .mutex = PTHREAD_MUTEX_INITIALIZER
    };

    return controller;
}

struct InputAxis *ControllerKeyAxis(struct Controller *controller, uint16_t key_code)
{
    uint16_t *axis_code = DictGet(controller->keybinds, &key_code);
    if(axis_code == 0)
        return 0;

    return DictGet(controller->axes, axis_code);
}

struct InputAxis *ControllerCodeAxis(struct Controller *controller, enum Control axis_code)
{
    return DictGet(controller->axes, &axis_code);
}

time_t START_TIME = 0;

void TimeInit()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    START_TIME = ts.tv_sec;
}

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec - START_TIME + (double)ts.tv_nsec / 1000000000.0;
}