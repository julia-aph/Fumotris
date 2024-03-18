#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include "win.h"
#endif

#define null 0

static const size_t IO_BUF_SIZE = 8;

enum Event {
    KEY,
    RESIZE,
    ESCAPE
};

struct KeyEvent {
    uint16_t key;
    bool is_down;
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

void block_input(void *args)
{
    struct InputEvent event_buf[IO_BUF_SIZE];

input_loop:
    #ifdef _WIN32
    bool success = WindowsBlockInput(event_buf);
    #endif

    if (!success)
        exit(1);

    goto input_loop;
}

void StartInput()
{
    pthread_t input_thread;
    pthread_create(&input_thread, null, block_input, 0);
}