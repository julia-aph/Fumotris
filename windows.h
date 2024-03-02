#include <stdint.h>
#include <windows.h>

struct Windows
{
    HANDLE standardInputHandle;
    HANDLE timer;
    INPUT_RECORD inputBuffer[32];
};

void WindowsInit(struct Windows *win);

void WindowsKeyEvent(KEY_EVENT_RECORD keyEvent, double timestamp, struct Controller *controller);

void WindowsResizeEvent(WINDOW_BUFFER_SIZE_RECORD windowEvent, struct Controller *controller);

void BlockWindowsInput(struct Windows *win, struct Controller *controller);

void WindowsWait(struct Windows *win, double seconds);