#include <stdint.h>

struct Buffer
{
    size_t width;
    size_t height;
    size_t area;
    
    char *chars;
    uint8_t **codeBuffer;
    uint8_t *codeBufferLengths;
};

struct Buffer *NewBuffer(size_t width, size_t height);

void DestroyBuffer(struct Buffer *buffer);