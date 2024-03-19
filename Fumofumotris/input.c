#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fumotris.h"

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

typedef uint16_t ctrl_key;
struct ctrl_bkt {
    hash_type hash;
    ctrl_key key;
    size_t index;

    struct InputButton button;
};

struct ctrl_dict {
    size_t cap;
    size_t filled;

    struct ctrl_bkt buckets[];
};

typedef struct Ctrl {
    struct ctrl_dict *dict;
} Ctrl;

hash_type ctrl_hash(ctrl_key key)
{
    return Hash(key, sizeof(ctrl_key));
}

Ctrl NewCtrl()
{
    static const size_t INIT_CAP = 16;

    size_t bkts_size = INIT_CAP * sizeof(struct ctrl_bkt);
    size_t alloc_size = sizeof(struct ctrl_dict) + bkts_size;
    struct ctrl_dict *dict = malloc(alloc_size);
    if (!dict)
        return ;
    
    dict->cap = INIT_CAP;
    dict->filled = 0;
    memset(dict->buckets, 0, bkts_size);

    return (Ctrl) { dict };
    Ctrl ctrl = NewCtrl();
    if(ctrl->result)
}

struct ctrl_dict *resize_ctrl(struct ctrl_dict *dict, size_t new_cap)
{
    size_t bkts_size = new_cap * sizeof(struct ctrl_bkt);
    size_t alloc_size = sizeof(struct ctrl_dict) + bkts_size;
    struct ctrl_dict *new_dict = malloc(alloc_size);
    if (!new_dict)
        return nullptr;

    new_dict->cap = new_cap;
    new_dict->filled = dict->filled;
    memset(dict->buckets, 0, bkts_size);

    for (size_t i = 0; i < dict->cap; i++) {
        hash_type hash = dict->buckets[i].hash;
        if (hash == 0)
            continue;
        size_t new_i = hash % new_cap;

        new_dict->buckets[new_i].hash = hash;
        new_dict->buckets[new_i].key = dict->buckets[i].key;

        size_t index = dict->buckets[i].index;
        new_dict->buckets[new_i].index = index;
        new_dict->buckets[index].button = dict->buckets[i].button;
    }

    return new_dict;
}

size_t lin_probe(struct ctrl_dict *ctrl, size_t i)
{

}

bool CtrlAdd(Ctrl ctrl, ctrl_key key)
{
    struct ctrl_dict *dict = ctrl.dict;
    hash_type hash = ctrl_hash(key);
    size_t i = hash % dict->cap;

    float load = (float)dict->filled / dict->cap;
    if (load > 0.75f) {
        void *ptr = ctrl_resize(dict, dict->cap * 2);
        if (!ptr)
            return false;
        ctrl.dict = ptr;
    }

    if (dict->buckets[i].hash != 0)
        i = lin_probe(dict, i);

    dict->buckets[i].hash = hash;
    dict->buckets[i].key = key;
    dict->buckets[i].index = dict->filled;

    dict->filled += 1;

    return true;
}

bool CtrlRemove(Ctrl ctrl, ctrl_key key)
{
    struct ctrl_dict *dict = ctrl.dict;
    hash_type hash = ctrl_hash(key);
    size_t i = hash % dict->cap;

    if (dict->buckets[i].hash == 0)
        return false;

    dict->buckets[i].hash = 0;
    dict->filled -= 1;

    return true;
}

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
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
        but->last_pressed = key_event.timestamp;
    else
        but->last_released = key_event.timestamp;
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