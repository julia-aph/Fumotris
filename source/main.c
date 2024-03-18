#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#include "win.h"
#endif

#include "buffer.h"
#include "blockmap.h"
#include "dictionary.h"
#include "input.h"
#include "gametime.h"
#include "str.h"

#include "config-TEMP.h"

struct Fumotris
{
    struct BlockMap *board;
    struct BlockMap *falling;
};

void Update(struct Fumotris game_state, struct Controller *ctrl)
{
    /*if(ControllerCodeAxis(ctrl, LEFT))
    {
        game_state.falling->x -= 1;
    }

    if(ControllerCodeAxis(ctrl, RIGHT))
    {
        game_state.falling->x += 1;
    }*/

    if(ControllerCodeAxis(ctrl, HARD_DROP))
    {
        game_state.falling->y += 1;
    }
}

void Draw(struct Fumotris game_state, struct Buffer *display_buf)
{
    DrawBlockMapToBuffer(game_state.board, display_buf, display_buf->width / 2);
    DrawBlockMapToBuffer(game_state.falling, display_buf, display_buf->width / 2);

    char *out = DrawBufferToString(display_buf);
    puts(out);
}

void *UpdateThread(void *args)
{
    struct Controller *ctrl = (struct Controller*)args;

    struct Fumotris game_state;
    game_state.board = NewBlockMap(10, 10);
    game_state.falling = NewBlockMapFrom(4, 4, I);

    struct Buffer *display_buf = NewBuffer(20, 10);

    while(true)
    {
        ControllerLockRead(ctrl);
        Update(game_state, ctrl);
        ControllerUnlockRead(ctrl);

        Draw(game_state, display_buf);

        Wait(0.1);
    }

    return 0;
}

void *InputThread(void *args)
{
    struct Controller *ctrl = (struct Controller*)args;

    while(1)
    {
        #ifdef _WIN32
        WindowsBlockInput(ctrl);
        #endif

        if(ControllerCodeAxis(ctrl, ESC)->is_down)
            break;
    }

    return 0;
}

int main()
{
    TimeInit();
    BlockColorsInit(BLOCK_COLORS, BLOCK_COLORS_COUNT);

    #ifdef _WIN32
    WindowsInit();
    #endif

    struct Controller main_ctrl = NewController(KEYS, CODES, CONTROL_COUNT);
    pthread_t input_thread;
    pthread_create(&input_thread, 0, InputThread, &main_ctrl);

    pthread_t update_thread;
    pthread_create(&update_thread, 0, UpdateThread, &main_ctrl);

    pthread_join(input_thread, 0);

    return 0;
}