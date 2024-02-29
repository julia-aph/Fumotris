#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

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

struct String *NewStringCopy(char *chars, int length)
{
    struct String *string = malloc(sizeof(struct String));
    
    string->chars = malloc(length + 1 * sizeof(char));
    Copy(string->chars, chars, length);
    string->chars[length] = 0;
    
    string->length = length;
    string->allocated = length + 1;

    return string;
}

struct String *NewStringFrom(char *chars, int length)
{
    struct String *string = malloc(sizeof(struct String));

    string->chars = chars;
    string->length = length;
    string->allocated = length + 1;
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

    int x;
    int y;

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

    blockMap->blocks = blocks;

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

    ConcatChars(string, "\x1b[H", 3);
    Concat(string, GenerateColorCode( &(unsigned char) {0}, 1 ));

    unsigned char lastColor = 0;
    for(int i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0 && i != 0 && i != buffer->area - 1)
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

    static int frame = 0;
    static double lastFrameTime = 0;

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    double now = (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);

    char fun[20] = { 0 };

    int bitch = sprintf(fun, " %d %f", frame, now - lastFrameTime);

    ConcatChars(string, fun, bitch);

    frame++;
    lastFrameTime = now;

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
    int bufferBlockWidth = buffer->width / 2;

    for(int i = 0; i < blockMap->area; i++)
    {
        int mapIndex = RotateIndex(i, blockMap->width, blockMap->height, blockMap->rotation);
        int bufferIndex = 
            i / blockMap->width * bufferBlockWidth
            + i % blockMap->width
            + blockMap->y * bufferBlockWidth
            + blockMap->x;

        if(bufferIndex < 0 || bufferIndex >= buffer->area / 2)
            continue;

        printf("%d ", bufferIndex);

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

unsigned int Hash(void *item, int size)
{
    unsigned char* data = (unsigned char*)item;

    unsigned int h = 98317;

    for(int i = 0; i < size; i++)
    {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }

    return h;
}

struct dictBucket
{
    void *key;
    unsigned int hash;
    void *value;
};

struct Dictionary
{
    struct dictBucket *buckets;
    unsigned int capacity;
    unsigned int used;

    unsigned int keySize;
};

struct Dictionary *NewDictionaryAllocate(unsigned int capacity, unsigned int keySize)
{
    struct Dictionary *dict = malloc(sizeof(struct Dictionary));
    dict->buckets = calloc(capacity, sizeof(struct dictBucket));
    dict->capacity = capacity;
    dict->used = 0;

    dict->keySize = keySize;
}

struct Dictionary *NewDictionary(unsigned int keySize)
{
    return NewDictionaryAllocate(16, keySize);
}

int dictLinearProbe(struct Dictionary *dict, int index, int search)
{
    if(dict->buckets[index].hash == search)
        return index;

    for(int i = 1; i < dict->capacity; i++)
    {
        unsigned int check = (index + i / 2 - (i % 2) * i) % dict->capacity;
        if(dict->buckets[check].hash == search)
            return check;
    }

    return -1;
}

void DictAdd(struct Dictionary *dict, void *key, void *value)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, 0);
    if(probe == -1)
        return; // Dictionary is full

    dict->buckets[probe] = (struct dictBucket){ key, hash, value };
    dict->used++;
}

/*void DictRemove(struct Dictionary *dict, void *key) // Make compatible with linear probe
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;
    dict->buckets[index].value = 0;
}*/

void *DictGet(struct Dictionary *dict, void *key)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, hash);
    if(probe == -1)
        return 0; // Not found

    return dict->buckets[probe].value;
}

unsigned char DictContainsKey(struct Dictionary *dict, void *key)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, hash);
    return probe == -1;
}

struct LinkedList
{
    int valueSize;
    void **head;
    unsigned int length;
};

struct LinkedList *NewLinkedList(int valueSize)
{
    struct LinkedList *list = malloc(sizeof(struct LinkedList));

    list->valueSize = valueSize;
    list->head = 0;
    list->length = 0;
}

void *LinkedListGet(struct LinkedList* list, int index)
{
    void **current = list->head;

    for(int i = 0; i < index; i++)
    {
        current = *current;
    }

    return current;
}

void *newLinkedListNode(struct LinkedList *list, char *value)
{
    char *data = calloc(sizeof(void*) + list->valueSize, 1);
    for(int i = 0; i < list->valueSize; i++)
    {
        data[sizeof(void*) + i] = value[i];
    }

    void **pointer = (void**)data;
    pointer = 0;

    return data;
}

void LinkedListAdd(struct LinkedList *list, void *value)
{
    if(list->head == 0)
    {
        list->head = newLinkedListNode(list, value);
    }

    
}

struct InputAxis
{
    unsigned char isPressed;
    double timeLastPressed;
};

struct Controller
{
    struct Dictionary *keybinds; // char: int
    struct Dictionary *axes; // int: inputAxis*
    struct InputAxis *inputAxes;

    pthread_mutex_t mutex;
};

struct Controller *NewController(char *keys, int *codes, int count)
{
    struct Controller *controller = malloc(sizeof(struct Controller));

    struct Dictionary *keybinds = NewDictionaryAllocate(count, sizeof(unsigned char));
    struct Dictionary *axes = NewDictionaryAllocate(count, sizeof(int));
    struct InputAxis *inputAxes = calloc(count, sizeof(struct InputAxis));
    for(int i = 0; i < count; i++)
    {
        DictAdd(keybinds, keys + i, codes + i);
        DictAdd(axes, codes + i, &inputAxes[i]);
    }

    controller->keybinds = keybinds;
    controller->axes = axes;
    controller->inputAxes = inputAxes;

    controller->mutex = PTHREAD_MUTEX_INITIALIZER;

    return controller;
}

struct InputAxis *ControllerGetAxis(struct Controller *controller, char key)
{
    int *code = DictGet(controller->keybinds, &key);
    if(code == 0)
        return 0;

    return DictGet(controller->axes, code);
}

struct InputAxis *ControllerAxis(struct Controller *controller, int code)
{
    return DictGet(controller->axes, &code);
}

double GetTime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}

enum Controls
{
    LEFT,
    RIGHT,
    SOFT_DROP,
    HARD_DROP,
    ROTATE_CCW,
    ROTATE_CW,
    ROTATE_180,
    SWAP,
    ESC
};

struct Windows
{
    HANDLE standardInputHandle;
    HANDLE timer;
    INPUT_RECORD inputBuffer[32];
};

void WindowsInit(struct Windows *win)
{
    win->standardInputHandle = GetStdHandle(STD_INPUT_HANDLE);
    if(win->standardInputHandle == INVALID_HANDLE_VALUE)
        exit(1);

    win->timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if(!win->timer)
        exit(1);

    BOOL consoleModeOk = SetConsoleMode(win->standardInputHandle, ENABLE_WINDOW_INPUT);
    if(!consoleModeOk)
        exit(1);

    system("color");
}

void WindowsKeyEvent(KEY_EVENT_RECORD keyEvent, double timestamp, struct Controller *controller)
{
    char key;
    switch(keyEvent.wVirtualKeyCode)
    {
        case VK_ESCAPE: key = 27; break;
        default: key = keyEvent.uChar.AsciiChar;
    }
    
    struct InputAxis *axis = ControllerGetAxis(controller, key);
    if(axis == 0) return;

    axis->isPressed = keyEvent.bKeyDown;
    if(keyEvent.bKeyDown)
    {
        axis->timeLastPressed = timestamp;
    }
}

void WindowsResizeEvent(WINDOW_BUFFER_SIZE_RECORD windowEvent, struct Controller *controller)
{

}

void BlockWindowsInput(struct Windows *win, struct Controller *controller)
{
    DWORD recordCount;
    BOOL readOk = ReadConsoleInput(win->standardInputHandle, win->inputBuffer, 32, &recordCount);
    if(!readOk)
        exit(1);

    double now = GetTime();

    pthread_mutex_lock(&controller->mutex);

    for (int i = 0; i < recordCount; i++)
    {
        if(win->inputBuffer[i].EventType == KEY_EVENT)
        {
            WindowsKeyEvent(win->inputBuffer[i].Event.KeyEvent, now, controller);
        }
        /*else if(win->inputBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT)
        {
            WindowsResizeEvent(win->inputBuffer[i].Event.WindowBufferSizeEvent, controller);
        }*/
    }

    pthread_mutex_unlock(&controller->mutex);
}

void WindowsWait(struct Windows *win, double seconds)
{
    LARGE_INTEGER duration;
    duration.QuadPart = (long)(-10000000.0 * seconds);

    SetWaitableTimer(win->timer, &duration, 0, NULL, NULL, FALSE);
    WaitForSingleObject(win->timer, INFINITE);
}

struct InputThreadArgs
{
    struct Windows *win;
    struct Controller *controller;
};

void *InputThread(void *inputThreadArgs)
{
    struct InputThreadArgs *args = inputThreadArgs;

    while(ControllerAxis(args->controller, ESC)->timeLastPressed == 0)
    {
        BlockWindowsInput(args->win, args->controller);
    }
}

struct String *ReadFile(char *path)
{
    FILE *file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = malloc(length + 1);
    fread(string, length, 1, file);
    fclose(file);

    string[length] = 0;

    return NewStringFrom(string, length);
}

void ParseJSON()
{
    
}

struct FumotrisGameState
{
    struct BlockMap board;
    struct BlockMap current;
};

void Run()
{
    struct BlockMap *board = NewBlockMap(10, 10);
    struct Buffer *displayBuffer = NewBuffer(20, 10);
}

int main()
{
    char defaultKeys[7] = { 'a', 'd', 'w', 's', 'q', 'e', 27 };
    int codes[7] = { LEFT, RIGHT, UP, DOWN, ROTATE_CCW, ROTATE_CW, ESC };

    struct Controller *controller = NewController(defaultKeys, codes, 7);

    struct Windows win = { 0 };
    WindowsInit(&win);


    struct InputThreadArgs args = (struct InputThreadArgs){ &win, controller };
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, *InputThread, &args);


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


    puts("\x1b[2J");
    while(1)
    {
        DrawBlockMapToBuffer(board, displayBuffer);
        DrawBlockMapToBuffer(a, displayBuffer);

        struct String *out = DrawBufferToString(displayBuffer);
        puts(out->chars);

        WindowsWait(&win, 1.0 / 30);


        pthread_mutex_lock(&controller->mutex);

        if(ControllerAxis(controller, LEFT)->isPressed)
            a->x--;
        if(ControllerAxis(controller, RIGHT)->isPressed)
            a->x++;
        if(ControllerAxis(controller, UP)->isPressed)
            a->y--;
        if(ControllerAxis(controller, DOWN)->isPressed)
            a->y++;

        if(ControllerAxis(controller, ROTATE_CCW)->isPressed)
            a->rotation = (unsigned char)(a->rotation - 1) % 4;
        if(ControllerAxis(controller, ROTATE_CW)->isPressed)
            a->rotation = (unsigned char)(a->rotation + 1) % 4;

        if(ControllerAxis(controller, ESC)->isPressed)
            break;

        pthread_mutex_unlock(&controller->mutex);
    }

    printf("done");
    return 0;
}