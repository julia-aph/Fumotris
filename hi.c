#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct String
{
    char *chars;
    unsigned int length;
    unsigned int allocated;
};

struct String *newStringFrom(char *chars, int length)
{
    struct String *string = malloc(sizeof(struct String));

    string->chars = chars;
    string->length = length;
    string->allocated = length + 1;

    return string;
}

struct String *newStringAllocate(int size)
{
    struct String *string = malloc(sizeof(struct String));

    string->chars = malloc(size * sizeof(char));
    string->length = 0;
    string->allocated = size;

    return string;
}

void stringAppendChars(struct String *destination, char *chars, int length)
{
    int newLength = destination->length + length;
    int newSize = newLength + 1;

    if(destination->allocated < newSize)
    {
        destination->chars = realloc(destination->chars, newSize);
        destination->allocated = newSize;
    }

    memcpy(destination->chars + destination->length, chars, length);
    destination->chars[newLength] = 0;
    destination->length = newLength;
}

void stringAppendChar(struct String *destination, char ch)
{
    int newLength = destination->length + 1;
    int newSize = newLength + 1;

    if(destination->allocated < newLength)
    {
        destination->chars = realloc(destination->chars, newLength);
        destination->allocated = newSize;
    }

    destination->chars[destination->length] = ch;
    destination->chars[newLength] = 0;
    destination->length = newLength;
}

void stringAppend(struct String *destination, struct String *source)
{
    stringAppendChars(destination, source->chars, source->length);
}

enum PieceType
{
    n,
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

struct String *generateColorCode(unsigned char color)
{
    int numLength = snprintf(NULL, 0, "%d", color);
    int stringLength = numLength + 3 + 1;

    char *colorString = malloc(stringLength);
    snprintf(colorString, stringLength, "\x1b[%dm", color);

    return newStringFrom(colorString, stringLength - 1);
}

struct String *drawToString(char *charBuffer, unsigned char *colorBuffer, int bufferWidth, int bufferHeight)
{
    int bufferSize = (bufferWidth * bufferHeight) + bufferHeight - 1;
    struct String *buffer = newStringAllocate(bufferSize);

    stringAppend(buffer, generateColorCode(0));

    for(int y = 0; y < bufferHeight; y++)
    {
        unsigned char lastColor = 0;
        for(int x = 0; x < bufferWidth; x++)
        {
            int index = y * bufferWidth + x;
            unsigned char newColor = colorBuffer[index];

            if(newColor != lastColor)
            {
                struct String *color = generateColorCode(newColor);
                stringAppend(buffer, color);

                free(color);
                lastColor = newColor;
            }

            stringAppendChars(buffer, charBuffer + index, 1);
        }

        if(y < bufferHeight-1)
        {
            stringAppendChars(buffer, "\n", 1);
        }
    }

    stringAppend(buffer, generateColorCode(0));

    return buffer;
}

void drawBlocksToBuffers(enum PieceType *blocks, char *charBuffer, unsigned char *colorBuffer, int blockWidth, int blockHeight, int width, int height, int x, int y)
{
    for(int localIndex = 0; localIndex < width * height; localIndex++)
    {
        int bufferIndex = (localIndex / width) * blockWidth;
        bufferIndex += localIndex % width;
        bufferIndex += y * blockWidth + x;

        int a = bufferIndex*2;
        int b = bufferIndex*2 + 1;
        
        if(blocks[localIndex] == n)
        {
            charBuffer[a] = '(';
            charBuffer[b] = ')';
        }
        else
        {
            charBuffer[a] = '[';
            charBuffer[b] = ']';
        }

        unsigned char color = 0;
        switch(blocks[localIndex])
        {
            case n: color = 90; break;
            case I: color = 96; break;
            case O: color = 93; break;
            case T: color = 95; break;
            case S: color = 92; break;
            case Z: color = 91; break;
            case J: color = 94; break;
            case L: color = 33; break;
        }

        colorBuffer[a] = color;
        colorBuffer[b] = color;
    }
}

struct ThreadChar
{
    char ch;
    unsigned char isRead;
    HANDLE mutex;
};

DWORD WINAPI InputThread(LPVOID lpParam)
{

}

DWORD WINAPI Update(LPVOID lpParam)
{
    
}

int main()
{
    int width = 10;
    int height = 10;
    int area = width * height;
    
    enum PieceType *board = malloc(area * sizeof(enum PieceType));
    for(int block = 0; block < area; block++) board[block] = n;


    int bufferWidth = width * 2;
    int bufferHeight = height;
    int bufferArea = bufferWidth * bufferHeight;

    char *charBuffer = calloc(bufferArea, sizeof(char));
    for(int i = 0; i < bufferArea; i++) charBuffer[i] = 'A';

    unsigned char *colorBuffer = malloc(bufferArea * sizeof(unsigned char));
    for(int color = 0; color < bufferArea; color++) colorBuffer[color] = 90;


    enum PieceType test[9] = {
        n, T, n,
        T, T, T,
        n, n, n
    };

    int testX = 0;
    int testY = 0;


    drawBlocksToBuffers(board, charBuffer, colorBuffer, width, height, width, height, 0, 0);
    drawBlocksToBuffers(test, charBuffer, colorBuffer, width, height, 3, 3, testX, testY);

    struct String *uh = drawToString(charBuffer, colorBuffer, bufferWidth, bufferHeight);

    printf("\e[1;1H\e[2J");
    puts(uh->chars);

    return 0;
}