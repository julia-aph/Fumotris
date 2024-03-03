#include <stdint.h>

struct BlockMap
{
    size_t width;
    size_t height;
    size_t area;

    int x;
    int y;

    uint8_t rotation;
    uint8_t *blocks;
};

struct BlockMap *NewBlockMap(size_t width, size_t height);

struct BlockMap *NewBlockMapFrom(size_t width, size_t height, uint8_t *blocks);

void DestroyBlockMap(struct BlockMap *blockMap);