#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <iso646.h>

struct String
{
    char *chars;
    unsigned int length;
    unsigned int allocated;
};

void Copy(char *dest, char *source, int size)
{
    for(int i = 0; i < size; i++)
    {
        dest[i] = source[i];
    }
}

struct String *NewStringFrom(char *chars, int length)
{
    struct String *string = malloc(sizeof(struct String));
    
    string->chars = malloc(length + 1 * sizeof(char));
    Copy(string->chars, chars, length);
    string->chars[length] = 0;
    
    string->length = length;
    string->allocated = length + 1;

    return string;
}

struct String *NewStringAllocate(int size)
{
    struct String *string = malloc(sizeof(struct String));

    string->chars = malloc(size * sizeof(char));
    string->length = 0;
    string->allocated = size;

    return string;
}

void ConcatChars(struct String *dest, char *chars, int length)
{
    int newLength = dest->length + length;
    int newSize = newLength + 1;

    if(dest->allocated < newSize)
    {
        dest->chars = realloc(dest->chars, newSize);
        dest->allocated = newSize;
    }

    Copy(dest->chars + length, chars, length);
    dest->chars[newLength] = 0;
    dest->length = newLength;
}

void ConcatChar(struct String *dest, char ch)
{
    ConcatChars(dest, &ch, 1);
}

void Concat(struct String *dest, struct String *source)
{
    ConcatChars(dest, source->chars, source->length);
}

void Insert(struct String *dest, char *chars, int length, int at)
{
    int newLength = dest->length + length;
    int newSize = newLength + 1;

    if(dest->allocated < newSize)
    {
        dest->chars = realloc(dest->chars, newSize);
        dest->allocated = newSize;
    }

    Copy(dest->chars + at + length, dest->chars + at, dest->length - at);
    Copy(dest->chars + at, chars, length);
    dest->chars[newLength] = 0;
    dest->length = newLength;
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

int FindDigits(unsigned char x)
{
    if(x < 10) return 1;
    if(x < 100) return 2;
    return 3;
}

struct String *GenerateColorCode(unsigned char *color)
{
    /*int digits = 0;
    
    int i = 0;
    for(; color[i] != 0; i++)
    {
        digits += FindDigits(color[i]);
    }

    sprintf(str, "%d", 42);
    struct String *colorString = NewStringAllocate(3 + digits + i - 2);

    ConcatChars(colorString, "\x1b[", 3);
    ConcatChars();

    struct String *colorString = NewStringFrom("\x1b[m", 3);

    //for(int i = 0; )
    int numLength = snprintf(NULL, 0, "%d", color);
    int stringLength = numLength + 3 + 1;

    char *colorString = malloc(stringLength);
    snprintf(colorString, stringLength, "\x1b[%dm", color);

    return NewStringFrom(colorString, stringLength - 1);*/
}

struct String *DrawToString(char *charBuffer, unsigned char *colorBuffer, int bufferWidth, int bufferHeight)
{
    int bufferSize = (bufferWidth * bufferHeight) + bufferHeight - 1;
    struct String *buffer = NewStringAllocate(bufferSize);

    Concat(buffer, GenerateColorCode(0));

    unsigned char lastColor = 0;
    for(int i = 0; i < bufferWidth * bufferHeight; i++)
    {
        if(i % bufferWidth == 0 and i != 0 and i != bufferWidth * bufferHeight - 1)
        {
            ConcatChar(buffer, '\n');
        }

        unsigned char newColor = colorBuffer[i];

        if(newColor != lastColor)
        {
            struct String *color = GenerateColorCode(newColor);
            Concat(buffer, color);

            free(color);
            lastColor = newColor;
        }

        ConcatChar(buffer, charBuffer[i]);
    }

    Concat(buffer, GenerateColorCode(0));

    return buffer;
}

void DrawBlocksToBuffers(enum PieceType *blocks, char *charBuffer, unsigned char *colorBuffer, int blockWidth, int blockHeight, int width, int height, int x, int y)
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

/*struct ThreadChar
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
    
}*/

int main()
{
    char str[] = "abcdef";
    sprintf(str, "%d", 42);
    
    printf("%s", str);

    return 0;
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

    enum PieceType test2[9] = {
        n, L, n,
        n, L, n,
        n, L, L
    };

    int testX = 0;
    int testY = 0;


    DrawBlocksToBuffers(board, charBuffer, colorBuffer, width, height, width, height, 0, 0);
    DrawBlocksToBuffers(test, charBuffer, colorBuffer, width, height, 3, 3, testX, testY);
    DrawBlocksToBuffers(test2, charBuffer, colorBuffer, width, height, 3, 3, 5, 5);

    struct String *uh = DrawToString(charBuffer, colorBuffer, bufferWidth, bufferHeight);

    //printf("\e[1;1H\e[2J");
    printf("%s", uh->chars);

    return 0;
}