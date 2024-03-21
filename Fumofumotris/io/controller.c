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

typedef u32 hash_type;
hash_type Hash(void *item, size_t size)
{
    u8* data = (u8*)item;

    u32 h = 98317;

    for (size_t i = 0; i < size; i++) {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }

    return h;
}

hash_type hash_ident(u16f ident, enum InputType type)
{
    
}

struct bkt {
    hash_type hash;
    u16 ident;
    size_t index;

    struct Axis axis;
};

/*
[hash, ident]   index   :   axis
[hash, ident]   index   :   axis
[hash, ident]   index   :   axis
*/

struct ctrl_dict {
    size_t capacity;
    size_t filled;

    struct bkt bkts; 
};

struct Ctrl {
    struct ctrl_dict *dict;
};
typedef struct Ctrl Ctrl;

