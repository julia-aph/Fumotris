#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include "win.h"
#endif

#define null 0

enum CtrlCode {
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

struct InputButton {
    bool is_down;
    double last_pressed;
    double last_released;
};

typedef uint32_t hash_type;
hash_type Hash(void *item, size_t size)
{
    uint8_t* data = (uint8_t*)item;

    uint32_t h = 98317;

    for(size_t i = 0; i < size; i++)
    {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }

    return h;
}

struct ctrl_bucket {
    hash_type hash;
    enum CtrlCode key;
    struct InputButton val;
};

struct Controller {
    size_t bucket_count;
    size_t filled;

    struct ctrl_bucket buckets[];
};

struct Controller *NewCtrl()
{
    static const size_t INIT_BUCKETS = 16;

    size_t buckets_size = sizeof(struct ctrl_bucket) * INIT_BUCKETS;

    struct Controller *ctrl = malloc(sizeof(struct Controller) + buckets_size);
    *ctrl = (struct Controller) {
        .bucket_count = INIT_BUCKETS, .filled = 0
    };
    memset(ctrl->buckets, 0, buckets_size);

    return ctrl;
}

void CtrlAdd(struct Controller *ctrl, enum CtrlCode key, struct InputButton val)
{
    hash_type hash = Hash(key, sizeof(enum CtrlCode));
    size_t i = hash % ctrl->bucket_count;

    ctrl->buckets[i] = (struct ctrl_bucket) {
        .hash = hash, .key = key, .val = val
    };
}

bool CtrlRemove(struct Controller *ctrl, enum CtrlCode key)
{
    hash_type hash = Hash(key, sizeof(enum CtrlCode));
    size_t i = hash % ctrl->bucket_count;

    if (ctrl->buckets[i].hash == 0)
        return false;
    
    memset(ctrl->buckets + i, 0, sizeof(struct ctrl_bucket));
    return true;
}



static const size_t IO_BUF_SIZE = 8;

enum Event {
    KEY,
    RESIZE,
    ESCAPE
};

struct KeyEvent {
    uint16_t key;
    bool is_down;
    double timestamp;
};

struct ResizeEvent {
    size_t new_x;
    size_t new_y;
};

struct InputEvent {
    enum Event type;
    union {
        struct KeyEvent key_event;
        struct ResizeEvent resize_event;
    };
};

void key_event(struct KeyEvent key_event)
{
    struct InputButton *but = CtrlGetButton(key_event.key);
    but->is_down = key_event.is_down;
    if (but->is_down)
        but->last_pressed = 
}

void resize_event(struct ResizeEvent resize_event)
{

}

void dispatch_input_event(struct InputEvent event)
{
    switch (event.type) {
    case KEY:
        return key_event(event.key_event);
    case RESIZE:
        return resize_event(event.resize_event);
    case ESCAPE:
        exit(1);
    }
}

void block_input(void *args)
{
    struct InputEvent event_buf[IO_BUF_SIZE];
    size_t count;

input_loop:
    #ifdef _WIN32
    bool success = WindowsBlockInput(event_buf, count);
    #endif
    if (!success)
        exit(1);

    for (size_t i = 0; i < count; i++) {
        dispatch_input_event(event_buf[i]);
    }

    goto input_loop;
}

void StartInput()
{
    pthread_t input_thread;
    pthread_create(&input_thread, null, block_input, 0);
}

double TimeNow()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec - GAME_START_SEC + (double)ts.tv_nsec / 1000000000.0;
}