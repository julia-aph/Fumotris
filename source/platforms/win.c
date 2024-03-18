#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

#include "input.h"
#include "gametime.h"

struct Windows
{
    HANDLE input_handle;
    HANDLE timer;
    INPUT_RECORD input_buffer[16];
};

static struct Windows WIN;

void WindowsInit()
{
    WIN.input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if(WIN.input_handle == INVALID_HANDLE_VALUE)
        exit(1);

    WIN.timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if(!WIN.timer)
        exit(1);

    BOOL console_mode_success = SetConsoleMode(WIN.input_handle, ENABLE_WINDOW_INPUT);
    if(!console_mode_success)
        exit(1);

    system("color");
}

void key_event(KEY_EVENT_RECORD key_event, double timestamp, struct Controller *controller)
{
    uint16_t key_code = key_event.wVirtualKeyCode;

    struct InputAxis *axis = ControllerKeyAxis(controller, key_code);
    if(axis == 0)
        return;

    axis->is_down = key_event.bKeyDown;

    if(key_event.bKeyDown)
        axis->last_time_down = timestamp;
}

void resize_event(WINDOW_BUFFER_SIZE_RECORD window_event, struct Controller *controller)
{

}

void WindowsBlockInput(struct Controller *ctrl)
{
    DWORD record_count;
    BOOL read_success = ReadConsoleInput(WIN.input_handle, WIN.input_buffer, 16, &record_count);
    if(!read_success)
        exit(1);

    double now = GetTime();

    ControllerLockWrite(ctrl);

    for(size_t i = 0; i < record_count; i++)
    {
        if(WIN.input_buffer[i].EventType == KEY_EVENT)
        {
            key_event(WIN.input_buffer[i].Event.KeyEvent, now, ctrl);
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

    SetWaitableTimer(WIN.timer, &duration, 0, NULL, NULL, FALSE);
    WaitForSingleObject(WIN.timer, INFINITE);
}