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
    struct Dictionary *keybinds; // int: int
    struct Dictionary *axes; // int: inputAxis*

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

struct Controller NewController(char *keys, int *codes, size_t count)
{
    static struct InputAxis default_input_axis = { 0, 0 };

    struct Dictionary *keybinds = NewDictionary(sizeof(char), sizeof(uint16_t));
    struct Dictionary *axes = NewDictionary(sizeof(uint16_t), sizeof(struct InputAxis));

    for(size_t i = 0; i < count; i++)
    {
        DictAdd(keybinds, &keys[i], &codes[i]);
        DictAdd(axes, &codes[i], &default_input_axis);
    }

    struct Controller controller = {
        .keybinds = keybinds, .axes = axes,

        .mutex = PTHREAD_MUTEX_INITIALIZER
    };

    return controller;
}

struct InputAxis *ControllerGetAxis(struct Controller *controller, char key)
{
    uint16_t *code = DictGet(controller->keybinds, &key);
    if(code == 0)
        return 0;

    return DictGet(controller->axes, code);
}

struct InputAxis *ControllerAxis(struct Controller *controller, uint16_t code)
{
    return DictGet(controller->axes, &code);
}

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}