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

void *DrawThread(void *args)
{
    struct BlockMap *board = NewBlockMap(10, 10);
    struct Buffer *display_buf = NewBuffer(20, 10);

    struct BlockMap *falling = NewBlockMapFrom(4, 4, I);

    while(1)
    {
        DrawBlockMapToBuffer(board, display_buf, display_buf->width / 2);
        DrawBlockMapToBuffer(falling, display_buf, display_buf->width / 2);

        char *out = DrawBufferToString(display_buf);
        puts(out);
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
    StartInputThread(&main_ctrl);

    pthread_t draw_thread;
    pthread_create(&draw_thread, 0, DrawThread, 0);

    pthread_join(draw_thread, 0);

    return 0;
}