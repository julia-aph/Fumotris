#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "fumotris.h"
#include "input.h"

struct Windows {
    HANDLE input_handle;
    HANDLE timer;
};
static struct Windows windows;

bool InitInputHandle()
{
    windows.input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if(windows.input_handle == INVALID_HANDLE_VALUE)
        return false;
    return true;
}

bool InitTimer()
{
    windows.timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if(!windows.timer)
        return false;
    return true;
}

bool InitConsoleMode()
{
    DWORD mode = ENABLE_WINDOW_INPUT 
        | ENABLE_MOUSE_INPUT
        | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return SetConsoleMode(windows.input_handle, mode);
}

void set_key_record(struct InputRecord *record, KEY_EVENT_RECORD win_key)
{
    record->type = KEY;
    record->id.key = win_key.wVirtualKeyCode;
    record->data.axis.is_down = win_key.bKeyDown;
}

bool set_mouse_record(struct InputRecord *record, MOUSE_EVENT_RECORD win_mouse)
{
    switch (win_mouse.dwEventFlags) {
    case MOUSE_WHEELED:
        record->type = AXIS;
        record->id.axis = 0;
        record->data.axis.value = win_mouse.dwButtonState;
        break;
    case MOUSE_HWHEELED:
        record->type = AXIS;
        record->id.axis = 1;
        record->data.axis.value = win_mouse.dwButtonState;
        break;
    case MOUSE_MOVED:
        record->type = JOYSTICK;
        record->id.joystick = 0;
        record->data.joystick.x = win_mouse.dwMousePosition.X;
        record->data.joystick.y = win_mouse.dwMousePosition.Y;
        break;
    default:
        return false;
    }
    return true;
}

void set_window_record(struct InputRecord *record, WINDOW_BUFFER_SIZE_RECORD win_resize)
{
    record->type = WINDOW;
    record->data.joystick.x = win_resize.dwSize.X;
    record->data.joystick.y = win_resize.dwSize.Y;
}

bool dispatch_record(struct InputRecord *record, INPUT_RECORD win_record)
{
    switch (win_record.EventType) {
    case KEY_EVENT:
        set_key_record(&record, win_record.Event.KeyEvent);
    case MOUSE_EVENT:
        return set_mouse_record(&record, win_record.Event.MouseEvent);
    case WINDOW_BUFFER_SIZE_EVENT:
        set_window_record(&record, win_record.Event.WindowBufferSizeEvent);
    default:
        record->type = ESCAPE;
    }
    return true;
}

bool GetInput(struct InputResult *result)
{
    INPUT_RECORD buf[8];
    DWORD count;

    bool success = ReadConsoleInput(windows.input_handle, buf, 8, &count);
    if (!success)
        return false;

    size_t unused_offset = 0;
    for (size_t i = 0; i < count; i++) {
        struct InputRecord record;
        bool include = dispatch_record(&record, buf[i]);

        if(record.type == ESCAPE)
            return false;
        if(!include)
            unused_offset += 1;

        result->buf[i - unused_offset] = record;
    }
    result->count = count - unused_offset;
    return true;
}

bool Wait(u64 nanoseconds)
{
    LARGE_INTEGER duration;
    duration.QuadPart = nanoseconds * 100;

    if(!SetWaitableTimer(windows.timer, &duration, 0, NULL, NULL, FALSE))
        return false;
    
    DWORD result = WaitForSingleObject(windows.timer, INFINITE);
    if(result != WAIT_OBJECT_0)
        return false;

    return true;
}