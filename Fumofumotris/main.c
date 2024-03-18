#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define nullptr ((void*)0)

struct CharBlock {
    char ch;
    uint8_t bg;
    uint8_t fg;
};

struct TermBuffer {
    size_t wid;
    size_t hgt;
    size_t area;

    struct CharBlock blocks[];
};

size_t TermBufferMaxChars(struct TermBuffer *term)
{
    return term->area + term->hgt - 1 + term->area * 10 + 8;
}

struct TermBuffer *NewTermBuffer(size_t wid, size_t hgt)
{
    size_t area = wid * hgt;

    struct TermBuffer *buf = malloc(sizeof(struct TermBuffer) + area * sizeof(struct CharBlock));

    *buf = (struct TermBuffer){
        .wid = wid, .hgt = hgt, .area = area
    };
    memset(buf->blocks, 0, area * sizeof(struct CharBlock));

    return buf;
}

size_t TermBufferToStringBuffer(struct TermBuffer *term, char *buf, size_t str_size)
{
    uint8_t last_bg = 0;
    uint8_t last_fg = 0;

    size_t filled = snprintf(buf, str_size, "\x1b[0m");

    for(size_t y = term->hgt - 1;; y--)
    {
    for(size_t x = 0; x < term->wid; x++)
    {
        size_t i = y * term->hgt + x;
        struct CharBlock *block = &term->blocks[i];

        uint8_t bg = 0;
        if(block->bg != 0 and block->bg != last_bg)
            bg = block->bg;

        uint8_t fg = 0;
        if(block->fg != 0 and block->fg != last_fg)
            fg = block->fg;

        if(bg == 0 and fg == 0) {
            buf[filled] = block->ch;
            filled += 1;
        }
        else if(bg != 0 and fg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%u;%um%c", bg, fg, block->ch);

            last_bg = bg;
            last_fg = fg;
        } else if(bg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%um%c", bg, block->ch);
        } else if(fg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%um%c", fg, block->ch);
        }
    }
        if(y == 0)
            break;

        buf[filled] = '\n';
        filled += 1;
    }

    filled += snprintf(buf + filled, str_size - filled, "\x1b[0m");

    buf[filled] = 0;
    return filled;
}

struct TetrMap {
    size_t wid;
    size_t hgt;
    size_t area;

    int x;
    int y;
    uint8_t rot;

    uint8_t blocks[];
};

struct TetrMap *NewTetrMap(size_t wid, size_t hgt)
{
    size_t area = wid * hgt;

    struct TetrMap *map = malloc(sizeof(struct TetrMap) + area);
    *map = (struct TetrMap){
        .wid = wid, .hgt = hgt, .area = area,

        .x = 0, .y = 0
    };
    memset(map->blocks, 0, area);

    return map;
}

void DrawTetrMapToTermBuffer(struct TetrMap *map, struct TermBuffer *term)
{
    static uint8_t BLOCK_COLORS[8] = { 90, 96, 93, 95, 92, 91, 94, 33 };

    for(size_t y = 0; y < map->hgt; y++)
    {
    for(size_t x = 0; x < map->wid; x++)
    {
        size_t map_i = y * map->hgt + x;
        size_t buf_i = term->wid * (map->y + y) + (map->x + x) * 2;

        struct CharBlock *a = &term->blocks[buf_i];
        struct CharBlock *b = &term->blocks[buf_i + 1];

        if(map->blocks[map_i] == 0) {
            a->ch = '(';
            b->ch = ')';
        } else {
            a->ch = '[';
            b->ch = ']';
        }

        uint8_t fg = BLOCK_COLORS[map->blocks[map_i]];
        a->fg = fg;
        b->fg = fg;
    }
    }
}

int main()
{
    struct TermBuffer *disp_term = NewTermBuffer(20, 10);

    size_t disp_buf_max = TermBufferMaxChars(disp_term) + 1;
    char *disp_buf = malloc(disp_buf_max);

    for(size_t i = 0; i < disp_term->area; i++)
    {
        disp_term->blocks[i].ch = '#';
    }

    struct TetrMap *board = NewTetrMap(10, 10);
    DrawTetrMapToTermBuffer(board, disp_term);

    TermBufferToStringBuffer(disp_term, disp_buf, disp_buf_max);

    puts(disp_buf);

    return 0;
}