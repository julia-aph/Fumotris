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
#include "str.h"

#include "config-TEMP.h"

int FindDigits(uint8_t x)
{
    if(x < 10) return 1;
    if(x < 100) return 2;
    return 3;
}

char *GenerateColorCode(uint8_t *codes, size_t count)
{
    int digits = 0;
    
    for(size_t i = 0; i < count; i++)
    {
        digits += FindDigits(codes[i]);
    }

    char *color_string = NewStringAlloc(
        2 
        + digits 
        + count-1
        + 1
    );

    ConcatChars(&color_string, "\x1b[");

    for(size_t i = 0; i < count; i++)
    {
        char numbers[4] = { 0 };
        sprintf(numbers, "%u", codes[i]);
        ConcatChars(&color_string, numbers);

        if(i != count-1)
        {
            ConcatChars(&color_string, ";");
        }
    }
    ConcatChars(&color_string, "m");

    return color_string;
}

char *DrawBufferToString(struct Buffer *buffer)
{
    static uint8_t reset_code = 0;

    size_t string_size = buffer->area + buffer->height - 1;
    char *string = NewStringAlloc(string_size);

    ConcatChars(&string, "\x1b[H");

    ConcatStr(&string, GenerateColorCode(&reset_code, 1));

    uint8_t last_color = 0;

    for(size_t i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0 && i != 0 && i != buffer->area - 1)
            ConcatChar(&string, '\n');

        uint8_t new_color = *buffer->pixels[i].codes;

        if(new_color != last_color)
        {
            char *color_string = GenerateColorCode(buffer->pixels[i].codes, buffer->pixels[i].code_count);
            ConcatStr(&string, color_string);
            DestroyString(color_string);
            last_color = new_color;
        }

        ConcatChar(&string, buffer->pixels[i].ch);
    }

    ConcatStr(&string, GenerateColorCode(&reset_code, 1));

    return string;
}

size_t RotateIndex(size_t i, size_t width, size_t height, uint8_t rotation)
{
    if(rotation == 0) return i;

    size_t row = i / width;
    size_t col = i % width;
    
    switch(rotation)
    {
        case 1:
            return (width - col - 1) * height + row;
        case 2:
            return (height - row - 1) * width + (width - col - 1);
        default:
            return col * height + (height - row - 1);
    }
}

void DrawBlockMapToBuffer(struct BlockMap *map, struct Buffer *buffer)
{
    size_t buffer_block_width = buffer->width / 2;

    for(size_t i = 0; i < map->area; i++)
    {
        size_t map_index = RotateIndex(i, map->width, map->height, map->rotation);
        size_t buffer_index = 
            i / map->width * buffer_block_width
            + i % map->width
            + map->y * buffer_block_width
            + map->x;

        if(buffer_index < 0 || buffer_index >= buffer->area / 2)
            continue;
        
        size_t a = buffer_index * 2;
        size_t b = buffer_index * 2 + 1;

        if(map->blocks[map_index] == 0)
        {
            buffer->pixels[a].ch = '(';
            buffer->pixels[b].ch = ')';
        }
        else
        {
            buffer->pixels[a].ch = '[';
            buffer->pixels[b].ch = ']';
        }

        buffer->pixels[a].codes[0] = 90;
        buffer->pixels[b].codes[0] = 90;
    }
}

void *InputThread(void *args)
{
    struct Controller *controller = (struct Controller*)args;

    while(1)
    {
        #ifdef _WIN32
        WindowsBlockInput(controller);
        #endif

        if(ControllerCodeAxis(controller, ESC)->is_down)
            break;
    }

    return 0;
}

void *DrawThread(void *args)
{
    struct Dictionary *piece_types = NewDictionary(1, sizeof(struct BlockMap*));
    struct BlockMap *maps[7] = {
        NewBlockMapFrom(4, 4, (uint8_t){
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0
        }),
        NewBlockMapFrom(2, 2, (uint8_t){
            1, 1,
            1, 1
        }),
        NewBlockMapFrom(3, 3, (uint8_t){
            0, 1, 0,
            1, 1, 1,
            0, 0, 0
        }),
        NewBlockMapFrom(3, 3, (uint8_t){
            0, 1, 1,
            1, 1, 0,
            0, 0, 0
        }),
        NewBlockMapFrom(3, 3, (uint8_t){
            1, 1, 0,
            0, 1, 1,
            0, 0, 0
        }),
        NewBlockMapFrom(3, 3, (uint8_t){
            1, 0, 0,
            1, 1, 1,
            0, 0, 0
        }),
        NewBlockMapFrom(3, 3, (uint8_t){
            0, 0, 1,
            1, 1, 1,
            0, 0, 0
        })
    };

    struct Dictionary *piece_colors = NewDictionary(1, sizeof(uint8_t*));
    uint8_t colors[7] = { 96, 93, 95, 92, 91, 94, 33 };

    for(uint8_t i = 0; i < 7; i++)
    {
        DictAdd(piece_types, i + 1, maps[i]);
        DictAdd(piece_colors, i + 1, colors[i]);
    }

    while(1)
    {        
        struct BlockMap *board = NewBlockMap(10, 10);
        struct Buffer *display_buffer = NewBuffer(20, 10);
        DrawBlockMapToBuffer(board, display_buffer);

        char *out = DrawBufferToString(display_buffer);
        puts(out);

        #ifdef _WIN32
        WindowsWait(0.1);
        #endif
    }

    return 0;
}

int main()
{
    TimeInit();

    #ifdef _WIN32
    WindowsInit();
    #endif

    struct Controller controller = NewController(KEYS, CODES, CONTROL_COUNT);

    pthread_t input_thread;
    pthread_create(&input_thread, 0, InputThread, &controller);

    pthread_t draw_thread;
    pthread_create(&draw_thread, 0, DrawThread, 0);

    pthread_join(input_thread, 0);

    return 0;
}