#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Pixel
{
    char ch;
    uint8_t *codes;
    uint8_t code_count;
};

struct Buffer
{
    size_t width;
    size_t height;
    size_t area;
    
    struct Pixel pixels[];
};

struct Buffer *NewBuffer(size_t width, size_t height)
{
    static struct Pixel default_pixel = { '#', 0, 1 };

    size_t area = width * height;

    struct Buffer *buffer = malloc(sizeof(struct Buffer) + sizeof(struct Pixel) * area);
    memset(buffer->pixels, 0, sizeof(struct Pixel) * area);

    // Initialize each character with 1 code
    for(size_t i = 0; i < area; i++)
    {
        buffer->pixels[i] = default_pixel;
        buffer->pixels[i].codes = malloc(sizeof(uint8_t));
        *buffer->pixels[i].codes = 95;
    }

    *buffer = (struct Buffer)
    {
        .width = width, .height = height, .area = area,
    };

    return buffer;
}

void DestroyBuffer(struct Buffer *buffer)
{
    for(size_t i = 0; i < buffer->area; i++)
    {
        free(buffer->pixels[i].codes);
    }

    free(buffer);
}