#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

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

    for(size_t i = 0; i < recordCount; i++)
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
    duration.QuadPart = (uint64_t)(-10000000.0 * seconds);

    SetWaitableTimer(win->timer, &duration, 0, NULL, NULL, FALSE);
    WaitForSingleObject(win->timer, INFINITE);
}