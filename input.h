#include <stdint.h>
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

struct Controller *NewController(char *keys, int *codes, int count);

struct InputAxis *ControllerGetAxis(struct Controller *controller, char key);

struct InputAxis *ControllerAxis(struct Controller *controller, uint16_t code);

double GetTime();

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