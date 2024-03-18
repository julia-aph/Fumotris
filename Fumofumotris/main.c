#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

struct CharBlock
{
    char ch;
    uint8_t fg;
    uint8_t bg;
};

struct TermBuffer
{
    size_t wid;
    size_t hgt;
    size_t area;

    struct CharBlock blocks[];
};

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

char *TermBufferToString(struct TermBuffer *buf)
{
    
}

int main()
{
    struct TermBuffer *display_buf = NewTermBuffer(20, 10);

    return 0;
}