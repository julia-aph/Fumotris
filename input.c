#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

struct InputAxis
{
    uint8_t isPressed;
    double timeLastPressed;
};

struct Controller
{
    struct Dictionary *keybinds; // char: u16
    struct Dictionary *axes; // u16: inputAxis*
    struct InputAxis *inputAxes;

    pthread_mutex_t mutex;
};

struct Controller *NewController(char *keys, int *codes, int count)
{
    struct Controller *controller = malloc(sizeof(struct Controller));

    struct Dictionary *keybinds = NewDictionaryAllocate(count, sizeof(uint8_t));
    struct Dictionary *axes = NewDictionaryAllocate(count, sizeof(uint16_t));
    struct InputAxis *inputAxes = calloc(count, sizeof(struct InputAxis));
    for(size_t i = 0; i < count; i++)
    {
        DictAdd(keybinds, keys + i, codes + i);
        DictAdd(axes, codes + i, &inputAxes[i]);
    }

    controller->keybinds = keybinds;
    controller->axes = axes;
    controller->inputAxes = inputAxes;

    controller->mutex = PTHREAD_MUTEX_INITIALIZER;

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