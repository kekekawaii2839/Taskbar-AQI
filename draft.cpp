#define DEBUG

#include "taskbarWindow.h"

#ifdef DEBUG
#include <iostream>
#endif

taskbarWindow* w;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // case WM_DESTROY:
        //     w->~taskbarWindow();
        //     PostQuitMessage(0);
        //     return 0;
        // case WM_PAINT:
        //     w->Paint();
        //     return 0;
        // case WA_ACTIVE:
        //     w->Paint();
        //     return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char* CLASS_NAME = "MyGO!!!!!";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    w = new taskbarWindow("MyGO!!!!!", "MyGO!!!!!", 200, hInstance, nCmdShow);
    w->ShowWindow();

    // main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
