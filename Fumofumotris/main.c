#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "win.h"

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

    size_t max_size;

    struct CharBlock blocks[];
};

struct TermBuffer *NewTermBuffer(size_t wid, size_t hgt)
{
    size_t area = wid * hgt;

    struct TermBuffer *buf = malloc(sizeof(struct TermBuffer) + area * sizeof(struct CharBlock));

    *buf = (struct TermBuffer){
        .wid = wid, .hgt = hgt, .area = area,
        .max_size = area + (area * 10) + (hgt - 1) + 12
    };
    memset(buf->blocks, 0, area * sizeof(struct CharBlock));

    return buf;
}

size_t TermBufferToStringBuffer(struct TermBuffer *term, char *buf, size_t str_size)
{
    uint8_t last_bg = 0;
    uint8_t last_fg = 0;

    size_t filled = snprintf(buf, str_size, "\x1b[H\x1b[0m");

    for (size_t y = 0; y < term->hgt; y++) {
    for (size_t x = 0; x < term->wid; x++) {
        size_t i = y * term->wid + x;
        struct CharBlock *block = &term->blocks[i];

        //DEBUG
        if (block->ch == 0)
            block->ch = '#';
        //DEBUG

        uint8_t bg = 0;
        if (block->bg != 0 and block->bg != last_bg)
            bg = block->bg;

        uint8_t fg = 0;
        if (block->fg != 0 and block->fg != last_fg)
            fg = block->fg;

        if (bg == 0 and fg == 0) {
            buf[filled] = block->ch;
            filled += 1;
        }
        else if (bg != 0 and fg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%u;%um%c", bg, fg, block->ch);

            last_bg = bg;
            last_fg = fg;
        } else if (bg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%um%c", bg, block->ch);
        } else if (fg != 0) {
            filled += snprintf(buf + filled, str_size - filled, "\x1b[%um%c", fg, block->ch);
        }
    }
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
    static const uint8_t BLOCK_COLORS[8] = { 90, 96, 93, 95, 92, 91, 94, 33 };

    for (size_t y = 0; y < map->hgt; y++) {
    for (size_t x = 0; x < map->wid; x++) {
        size_t map_i = y * map->wid + x;
        size_t buf_i = (y + map->y) * term->wid + (x + map->x) * 2;

        struct CharBlock *a = &term->blocks[buf_i];
        struct CharBlock *b = &term->blocks[buf_i + 1];

        if (map->blocks[map_i] == 0) {
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

bool TetrCollisionCheck(struct TetrMap *board, struct TetrMap *piece, int dx, int dy)
{
    size_t i = 0;
    for (size_t y = piece->y + dy; y < piece->y + piece->hgt + dy; y++) {
    for (size_t x = piece->x + dx; x < piece->x + piece->wid + dx; x++) {
        if(piece->blocks[i] == 0)
            goto next;

        if(y >= board->hgt or x >= board->wid)
            return false;

        size_t board_i = y * board->wid + x;
        if(board->blocks[board_i] != 0)
            return false;
next:
        i++;
    }
    }

    return true;
}

void Loop()
{
    struct TermBuffer *disp_term = NewTermBuffer(20, 20);
    char *disp_buf = malloc(disp_term->max_size);

    struct TetrMap *board = NewTetrMap(10, 20);
    struct TetrMap *falling = NewTetrMap(4, 4);

    uint8_t I[16] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 1, 1, 1,
        0, 0, 0, 0
    };
    for (size_t i = 0; i < 16; i++) {
        falling->blocks[i] = I[i];
    }

    for (int i = 0; i < 10; i++) {
        DrawTetrMapToTermBuffer(board, disp_term);
        DrawTetrMapToTermBuffer(falling, disp_term);

        TermBufferToStringBuffer(disp_term, disp_buf, disp_term->max_size);
        puts(disp_buf);

        falling->y += 1;

        WindowsWait(0.2);
    }
}

int main()
{
    WindowsInit();

    Loop();

    return 0;
}