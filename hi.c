#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iso646.h>
#include <time.h>

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
    string->chars[0] = 0;

    string->length = 0;
    string->allocated = size;

    return string;
}

void DestroyString(struct String *string)
{
    free(string->chars);

    free(string);
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

    Copy(dest->chars + dest->length, chars, length);
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

struct Buffer
{
    unsigned int width;
    unsigned int height;
    unsigned int area;

    char *chars;
    unsigned char **codeBuffer;
    unsigned char *codeBufferLengths;
};

struct Buffer *NewBuffer(unsigned int width, unsigned int height)
{
    struct Buffer *buffer = malloc(sizeof(struct Buffer));
    
    buffer->width = width;
    buffer->height = height;
    unsigned int area = width * height;
    buffer->area = area;

    char *charBuffer = malloc(area * sizeof(char));
    for(int ch = 0; ch < area; ch++)
    {
        // Initialize each character as #
        charBuffer[ch] = '#';
    }
    buffer->chars = charBuffer;

    unsigned char **codeBuffer = malloc(area * sizeof(unsigned char *));
    unsigned char *codeBufferLengths = malloc(area * sizeof(unsigned char));
    for(int i = 0; i < area; i++)
    {
        // Initialize each character with one code
        codeBuffer[i] = calloc(1, sizeof(unsigned char));
        codeBufferLengths[i] = 1;
    }
    buffer->codeBuffer = codeBuffer;
    buffer->codeBufferLengths = codeBufferLengths;

    return buffer;
}

void DestroyBuffer(struct Buffer *buffer)
{
    free(buffer->chars);
    for(int i = 0; i < buffer->area; i++)
    {
        free(buffer->codeBuffer[i]);
    }
    free(buffer->codeBuffer);
    free(buffer->codeBufferLengths);

    free(buffer);
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

struct BlockMap
{
    unsigned int width;
    unsigned int height;
    unsigned int area;

    unsigned int x;
    unsigned int y;

    unsigned char rotation;

    enum PieceType *blocks;
};

struct BlockMap *NewBlockMap(unsigned int width, unsigned int height)
{
    struct BlockMap *blockMap = malloc(sizeof(struct BlockMap));

    blockMap->width = width;
    blockMap->height = height;
    unsigned int area = width * height;
    blockMap->area = area;

    blockMap->x = 0;
    blockMap->y = 0;
    blockMap->rotation = 0;

    enum PieceType *blocks = malloc(area * sizeof(enum PieceType));
    // Fill with default value
    for(int i = 0; i < area; i++)
    {
        blocks[i] = n;
    }
    blockMap->blocks = blocks;

    return blockMap;
}

struct BlockMap *NewBlockMapFrom(unsigned int width, unsigned int height, enum PieceType *blocks)
{
    struct BlockMap *blockMap = malloc(sizeof(struct BlockMap));

    blockMap->width = width;
    blockMap->height = height;
    unsigned int area = width * height;
    blockMap->area = area;

    blockMap->x = 0;
    blockMap->y = 0;
    blockMap->rotation = 0;

    enum PieceType *newBlocks = malloc(area * sizeof(enum PieceType));
    // Copy
    for(int i = 0; i < area; i++)
    {
        newBlocks[i] = blocks[i];
    }
    blockMap->blocks = newBlocks;

    return blockMap;
}

void DestroyBlockMap(struct BlockMap *blockMap)
{
    free(blockMap->blocks);
    free(blockMap);
}

int FindDigits(unsigned char x)
{
    if(x < 10) return 1;
    if(x < 100) return 2;
    return 3;
}

struct String *GenerateColorCode(unsigned char *codes, int count)
{
    int digits = 0;
    
    for(int i = 0; i < count; i++)
    {
        digits += FindDigits(codes[i]);
    }

    struct String *colorString = NewStringAllocate(
        2 
        + digits 
        + count-1
        + 1
    );

    ConcatChars(colorString, "\x1b[", 2);
    for(int i = 0; i < count; i++)
    {
        colorString->length = sprintf(colorString->chars, "%s%d", colorString->chars, codes[i]);
        if(i != count - 1)
        {
            ConcatChar(colorString, ';');
        }
    }
    ConcatChar(colorString, 'm');

    return colorString;
}

struct String *DrawBufferToString(struct Buffer *buffer)
{
    int stringSize = buffer->area + buffer->height - 1;
    struct String *string = NewStringAllocate(stringSize);

    Concat(string, GenerateColorCode( &(unsigned char) {0}, 1 ));

    unsigned char lastColor = 0;
    for(int i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0 and i != 0 and i != buffer->area - 1)
        {
            ConcatChar(string, '\n');
        }

        unsigned char newColor = buffer->codeBuffer[i][0];

        if(newColor != lastColor)
        {
            struct String *color = GenerateColorCode( &(unsigned char) {newColor}, 1 );
            Concat(string, color);
            DestroyString(color);
            lastColor = newColor;
        }

        ConcatChar(string, buffer->chars[i]);
    }

    Concat(string, GenerateColorCode( &(unsigned char) {0}, 1 ));

    return string;
}

int RotateIndex(int i, int width, int height, int rotation)
{
    if(rotation == 0) return i;

    int row = i / width;
    int col = i % width;
    
    switch(rotation)
    {
        case 1:
            return (width - col - 1) * height + row;
        case 2:
            return (height - row - 1) * width + (width - col - 1);
        case 3:
            return col * height + (height - row - 1);
    }
}

void DrawBlockMapToBuffer(struct BlockMap *blockMap, struct Buffer *buffer)
{
    int blockWidth = buffer->width / 2;

    for(int i = 0; i < blockMap->area; i++)
    {
        int mapIndex = RotateIndex(i, blockMap->width, blockMap->height, blockMap->rotation);
        int bufferIndex = 
            i / blockMap->width * blockWidth
            + i % blockMap->width
            + blockMap->y * blockWidth
            + blockMap->x;

        int a = bufferIndex * 2;
        int b = bufferIndex * 2 + 1;

        if(blockMap->blocks[mapIndex] == n)
        {
            buffer->chars[a] = '(';
            buffer->chars[b] = ')';
        }
        else
        {
            buffer->chars[a] = '[';
            buffer->chars[b] = ']';
        }

        unsigned char color = 0;
        switch(blockMap->blocks[mapIndex])
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

        buffer->codeBuffer[a][0] = color;
        buffer->codeBuffer[b][0] = color;
    }
}

void OverlayBlockMap(struct BlockMap *dest, struct BlockMap *source)
{
    for(int i = 0; i < source->area; i++)
    {
        int sourceIndex = RotateIndex(i, source->width, source->height, source->rotation);

        if(source->blocks[sourceIndex] == n)
            continue;

        int destIndex = 
            i / source->width * dest->width
            + i % source->width
            + source->y * dest->width
            + source->x;

        dest->blocks[destIndex] = source->blocks[sourceIndex];
    }
}

struct DictionaryBucket
{
    void *key;
    unsigned int hash;
    void *value;
};

struct Dictionary
{
    struct DictionaryBucket *buckets;
    unsigned int capacity;
    unsigned int used;
};

struct Dictionary *NewDictionaryAllocate(unsigned int capacity)
{
    struct Dictionary *dict = malloc(sizeof(struct Dictionary));
    dict->buckets = calloc(capacity, sizeof(struct DictionaryBucket));
    dict->capacity = capacity;
    dict->used = 0;
}

struct Dictionary *NewDictionary()
{
    return NewDictionaryAllocate(16);
}

void DictAdd(struct Dictionary *dict, void *key, unsigned int hash, void *value)
{
    unsigned int index = hash % dict->capacity;

    if(dict->buckets[index].hash != 0)
    {
        printf("collision: %c (%d)@i%d\n", *(char*)dict->buckets[index].key, dict->buckets[index].hash, index);
        printf("attempt:   %c (%d)\n", *(char*)key, hash);
    }
    else
    {
        dict->buckets[index].key = key;
        dict->buckets[index].hash = hash;
        dict->buckets[index].value = value;
        dict->used++;
    }
}

void DictRemove(struct Dictionary *dict, unsigned int hash)
{
    unsigned int index = hash % dict->capacity;
    dict->buckets[index].value = 0;
}

void *DictGet(struct Dictionary *dict, unsigned int hash)
{
    unsigned int index = hash % dict->capacity;
    return dict->buckets[index].value;
}

unsigned char DictContainsKey(struct Dictionary *dict, unsigned int hash)
{
    unsigned int index = hash % dict->capacity;
    return dict->buckets[index].value != 0;
}

unsigned int Hash(void *item, int size)
{
    char *p = (char*)item;

    unsigned int hash = 0;
    for(int i = 0; i < size; i++)
    {
        hash += p[i] * 31;
    }

    return hash;
}

unsigned int HashInt(int item) { return Hash(&item, sizeof(int)); }
unsigned int HashChar(char item) { return Hash(&item, sizeof(char)); }

enum Controls
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct InputAxis
{
    unsigned char isPressed;
    double timeLastPressed;
};

struct Controller
{
    struct Dictionary *keybinds; // char: int
    struct Dictionary *axes; // int: inputAxis

    struct InputAxis *inputAxes;
};

struct Controller *NewController(char *keys, int *codes, int count)
{
    struct Controller *controller = malloc(sizeof(struct Controller));

    struct Dictionary *keybinds = NewDictionaryAllocate(count);
    struct Dictionary *axes = NewDictionaryAllocate(count);
    struct InputAxis *inputAxes = calloc(count, sizeof(struct InputAxis));
    for(int i = 0; i < count; i++)
    {
        DictAdd(keybinds, keys + i, HashChar(keys[i]), codes + i);
        DictAdd(axes, codes + i, HashInt(codes[i]), inputAxes + i);
    }

    controller->keybinds = keybinds;
    controller->axes = axes;
    controller->inputAxes = inputAxes;

    return controller;
}

HANDLE standardInputHandle;
INPUT_RECORD inputBuffer[32];

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}

void WindowsInputInit()
{
    standardInputHandle = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(standardInputHandle, ENABLE_WINDOW_INPUT);
}

void WindowsKeyEvent(KEY_EVENT_RECORD keyEvent)
{
    
}

void WindowsInput()
{
    DWORD recordCount;
    ReadConsoleInput(
        standardInputHandle,    // input buffer handle
        inputBuffer,            // buffer to read into
        32,                     // size of read buffer
        &recordCount            // number of records read
    );

    double now = GetTime();

    for (int i = 0; i < recordCount; i++)
    {
        if(inputBuffer[i].EventType == KEY_EVENT)
        {
            WindowsKeyEvent(inputBuffer[i].Event.KeyEvent);
        }
        else if(inputBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT)
        {

        }
    }
}

int main()
{
    char defaultKeys[4] = { 'w', 'a', 's', 'd' };
    int codes[4] = { UP, LEFT, DOWN, RIGHT };
    struct Controller *controller = NewController(defaultKeys, codes, 4);

    struct InputAxis *axis = DictGet(controller->axes, HashInt(DOWN));

    for(int i = 0; i < controller->keybinds->capacity; i++)
    {
        char *k = (char*)controller->keybinds->buckets[i].key;
        int *v = (int*)controller->keybinds->buckets[i].value;
        int h = controller->keybinds->buckets[i].hash;

        char ko;
        if(k == 0)
            ko = '0';
        else
            ko = *k;

        int vo;
        if(v == 0)
            vo = 0;
        else
            vo = *v;

        printf("%c (%d): %d\n", ko, h, vo);
    }

    return 0;
    WindowsInputInit();

    /*struct Dictionary *inputAxes = NewDictionaryAllocate(4);
    DictAdd(inputAxes, HashInt(LEFT), 'a');
    DictAdd(inputAxes, HashInt(RIGHT), 'd');
    DictAdd(inputAxes, HashInt(UP), 'w');
    DictAdd(inputAxes, HashInt(DOWN), 's');*/
    return 0;

    struct BlockMap *board = NewBlockMap(10, 10);
    struct Buffer *displayBuffer = NewBuffer(20, 10);

    enum PieceType tPiece[9] = {
        n, T, n,
        T, T, T,
        n, n, n
    };

    enum PieceType lPiece[9] = {
        n, L, n,
        n, L, n,
        n, L, L
    };

    struct BlockMap *a = NewBlockMapFrom(3, 3, tPiece);
    a->rotation = 0; a->y = 0;


    while(1)
    {
        DrawBlockMapToBuffer(board, displayBuffer);
        DrawBlockMapToBuffer(a, displayBuffer);
        struct String *out = DrawBufferToString(displayBuffer);

        printf("\e[1;1H\e[2J");
        puts(out->chars);
    }

    return 0;
}