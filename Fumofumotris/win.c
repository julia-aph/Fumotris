#include <stdint.h>
#include <windows.h>

#include "input.h"

struct Windows
{
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

    BOOL success = SetConsoleMode(WIN.input_handle, ENABLE_WINDOW_INPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!success)
        return false;
}

struct InputEvent key_event(KEY_EVENT_RECORD key_event, double timestamp)
{   
    struct InputEvent event;

    if (key_event.wVirtualKeyCode == VK_ESCAPE) {
        event.type = ESCAPE;
        return event;
    }

    event.type = KEY,
    event.key_event = (struct KeyEvent) {
        .key = key_event.wVirtualKeyCode,
        .is_down = key_event.bKeyDown,
        .timestamp = timestamp
    };
    return event;
}

struct InputEvent resize_event(WINDOW_BUFFER_SIZE_RECORD window_event)
{
    struct InputEvent event;

    event.type = RESIZE,
    event.resize_event = (struct ResizeEvent) {
        .new_x = window_event.dwSize.X,
        .new_y = window_event.dwSize.Y
    };
    return event;
}

struct InputEvent dispatch_event(INPUT_RECORD input_record, double timestamp)
{
    switch(input_record.EventType) {
    case KEY_EVENT:
        return key_event(input_record.Event.KeyEvent, timestamp);
    case WINDOW_BUFFER_SIZE_EVENT:
        return resize_event(input_record.Event.WindowBufferSizeEvent);
    default:
        return (struct InputEvent) {
            .type = ESCAPE
        };
    }
}

bool WindowsBlockInput(struct InputEvent event_buf[IO_BUF_SIZE], size_t *results)
{
    static INPUT_RECORD input_buf[IO_BUF_SIZE];
    static DWORD count;

    BOOL success = ReadConsoleInput(WIN.input_handle, input_buf, IO_BUF_SIZE, &count);
    if (!success)
        return false;

    double timestamp = GetTime();

    for (size_t i = 0; i < count; i++) {
        struct InputEvent event = dispatch_event(input_buf[i], timestamp);
        if (event.type == ESCAPE)
            return false;

        event_buf[i] = event;
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