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

typedef uint16_t key_type;
struct ctrl_bkt {
    hash_type hash;
    key_type key;
    size_t but_index;

    struct InputButton button;
};

struct ctrl_dict {
    size_t capacity;
    size_t filled;

    struct ctrl_bkt buckets[];
};

typedef struct Ctrl {
    struct ctrl_dict *dict;
} Ctrl;

hash_type ctrl_hash(key_type key)
{
    return Hash(key, sizeof(key_type));
}

struct ctrl_dict *new_ctrl_dict(size_t cap, size_t filled)
{
    size_t bkts_size = cap * sizeof(struct ctrl_bkt);
    size_t alloc_size = sizeof(struct ctrl_dict) + bkts_size;
    struct ctrl_dict *dict = malloc(alloc_size);
    if(!dict)
        return nullptr;

    dict->capacity = cap;
    dict->filled = filled;
    memset(dict->buckets, 0, bkts_size);
}

Ctrl NewCtrl()
{
    static const size_t INIT_CAP = 16;

    struct ctrl_dict *dict = new_ctrl_dict(INIT_CAP, 0);
    return (Ctrl) { dict };
}

void set_bkt(struct ctrl_bkt *bkt, hash_type hash, key_type key, size_t but_index)
{
    bkt->hash = hash;
    bkt->key = key;
    bkt->but_index = but_index;
}

struct ctrl_bkt *get_bkt(struct ctrl_dict *dict, size_t i)
{
    return &dict->buckets[i];
}

void set_button(struct ctrl_dict *dict, size_t but_index, struct InputButton *button)
{
    dict->buckets[but_index].button = *button;
}

struct InputButton *get_button(struct ctrl_dict *dict, size_t but_index)
{
    return &dict->buckets[but_index].button;
}

struct ctrl_dict *ctrl_resize(struct ctrl_dict *dict, size_t new_cap)
{
    struct ctrl_dict *new_dict = new_ctrl_dict(new_cap, dict->filled);
    if(!new_dict)
        return nullptr;

    for (size_t i = 0; i < dict->capacity; i++) {
        struct ctrl_bkt *bkt = get_bkt(dict, i);
        if (bkt->hash == 0)
            continue;

        size_t new_i = bkt->hash % new_cap;
        struct ctrl_bkt *new_bkt = get_bkt(new_dict, new_i);

        set_bkt(new_bkt, bkt->hash, bkt->key, bkt->but_index);
        set_button(new_dict, bkt->but_index, get_button(dict, bkt->but_index));
    }

    return new_dict;
}

size_t lin_probe(struct ctrl_dict *ctrl, size_t i)
{

}

bool CtrlAdd(Ctrl ctrl, key_type key)
{
    struct ctrl_dict *dict = ctrl.dict;

    hash_type hash = ctrl_hash(key);
    size_t i = hash % dict->capacity;

    float load_factor = (float)dict->filled / dict->capacity;
    if (load_factor > 0.75f) {
        void *ptr = ctrl_resize(dict, dict->capacity * 2);
        if (!ptr)
            return false;
        ctrl.dict = ptr;
    }

    if (dict->buckets[i].hash != 0)
        i = lin_probe(dict, i);
    
    set_bkt(&dict->buckets[i], hash, key, dict->filled);
    dict->filled += 1;

    return true;
}

bool CtrlRemove(Ctrl ctrl, key_type key)
{
    struct ctrl_dict *dict = ctrl.dict;
    hash_type hash = ctrl_hash(key);
    size_t i = hash % dict->capacity;

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