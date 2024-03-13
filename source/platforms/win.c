#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

#include "input.h"

struct Windows
{
    HANDLE std_input_handle;
    HANDLE timer;
    INPUT_RECORD input_buffer[16];
};

static struct Windows win;

void WindowsInit()
{
    win.std_input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if(win.std_input_handle == INVALID_HANDLE_VALUE)
        exit(1);

    win.timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if(!win.timer)
        exit(1);

    BOOL console_mode_success = SetConsoleMode(win.std_input_handle, ENABLE_WINDOW_INPUT);
    if(!console_mode_success)
        exit(1);

    system("color");
}

void WindowsKeyEvent(KEY_EVENT_RECORD key_event, double timestamp, struct Controller *controller)
{
    uint16_t key_code = key_event.wVirtualKeyCode;

    struct InputAxis *axis = ControllerKeyAxis(controller, key_code);
    if(axis == 0)
        return;

    axis->is_down = key_event.bKeyDown;

    if(key_event.bKeyDown)
        axis->last_time_down = timestamp;
}

void WindowsResizeEvent(WINDOW_BUFFER_SIZE_RECORD window_event, struct Controller *controller)
{

}

void WindowsBlockInput(struct Controller *ctrl)
{
    DWORD record_count;
    BOOL read_success = ReadConsoleInput(win.std_input_handle, win.input_buffer, 16, &record_count);
    if(!read_success)
        exit(1);

    double now = GetTime();

    ControllerLockWrite(ctrl);

    for(size_t i = 0; i < record_count; i++)
    {
        if(win.input_buffer[i].EventType == KEY_EVENT)
        {
            WindowsKeyEvent(win.input_buffer[i].Event.KeyEvent, now, ctrl);
        }
        /*else if(win->inputBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT)
        {
            WindowsResizeEvent(win->inputBuffer[i].Event.WindowBufferSizeEvent, controller);
        }*/
    }

    ControllerUnlockWrite(ctrl);
}

void WindowsWait(double seconds)
{
    LARGE_INTEGER duration;
    duration.QuadPart = (uint64_t)(-10000000.0 * seconds);

    SetWaitableTimer(win.timer, &duration, 0, NULL, NULL, FALSE);
    WaitForSingleObject(win.timer, INFINITE);
}