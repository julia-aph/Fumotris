#include <stdint.h>
#include <windows.h>

#include "input.h"
#include "controller.h"

struct Windows {
    HANDLE input_handle;
    HANDLE timer;
};

static struct Windows WIN;

bool WindowsInit()
{
    WIN.input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (WIN.input_handle == INVALID_HANDLE_VALUE)
        return false;

    WIN.timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!WIN.timer)
        return false;

    DWORD mode = ENABLE_WINDOW_INPUT 
        | ENABLE_MOUSE_INPUT
        | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    BOOL success = SetConsoleMode(WIN.input_handle, mode);
    if (!success)
        return false;
}

bool key_event(struct InputUpdate *updt, KEY_EVENT_RECORD key_event)
{   
    if (key_event.wVirtualKeyCode == VK_ESCAPE)
        return false;

    updt->type = KEY;
    updt->ident.key = key_event.wVirtualKeyCode;
    updt->data.value = key_event.bKeyDown;

    updt->is_down = key_event.bKeyDown;

    return true;
}

struct InputUpdate mouse_dispatch(MOUSE_EVENT_RECORD mouse_event)
{
    struct InputUpdate update;

    switch (mouse_event.dwEventFlags) {
    case 0:
        update.type = MOUSE_BUTTON;

        if (mouse_event.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
            update.button = 0;
        }
        if (mouse_event.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
            update.button = 1;
        }
        
    case DOUBLE_CLICK:
        return;
    case MOUSE_WHEELED:
    
    case MOUSE_HWHEELED:
    case MOUSE_MOVED:
    default:
    }
}

struct InputUpdate resize_event(WINDOW_BUFFER_SIZE_RECORD window_event)
{
    struct InputUpdate update;

    update.type = WINDOW;
    update.x = window_event.dwSize.X;
    update.y = window_event.dwSize.Y;

    return update;
}

struct InputUpdate dispatch_record(INPUT_RECORD input_record)
{
    switch(input_record.EventType) {
    case KEY_EVENT:
        return key_event(input_record.Event.KeyEvent);
    case MOUSE_EVENT:
        return mouse_dispatch(input_record.Event.MouseEvent);
    case WINDOW_BUFFER_SIZE_EVENT:
        return resize_event(input_record.Event.WindowBufferSizeEvent);
    }
    return (struct InputUpdate) { .type = ESCAPE };
}

bool WindowsBlockInput(struct InputUpdate buf[IO_BUF_SIZE], size_t *results)
{
    static INPUT_RECORD rec_buf[IO_BUF_SIZE];
    static DWORD count;

    BOOL success = ReadConsoleInput(WIN.input_handle, rec_buf, IO_BUF_SIZE, &count);
    if (!success)
        return false;

    double timestamp = GetTime();

    for (size_t i = 0; i < count; i++) {


        struct InputUpdate update;
        struct InputUpdate update = dispatch_record(rec_buf[i]);

        if (update.type == ESCAPE)
            return false;

        update.timestamp = timestamp;

        buf[i] = update;
    }
    *results = count;
    return true;
}

void WindowsWait(double seconds)
{
    LARGE_INTEGER duration;
    duration.QuadPart = (uint64_t)(-10000000.0 * seconds);

    SetWaitableTimer(WIN.timer, &duration, 0, NULL, NULL, FALSE);
    WaitForSingleObject(WIN.timer, INFINITE);
}