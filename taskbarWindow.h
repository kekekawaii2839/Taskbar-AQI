#define WINVER 0x0605

#include <windows.h>
#include <gdiplus.h>
#include <dwrite.h>
#include <d2d1.h>

#ifdef DEBUG
#include <iostream>
#endif

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

class taskbarWindow {
private:
    HWND hwnd;
    int nCmdshow;
    int width;

    HWND m_hTaskbar;
    HWND m_hWin11Bridge;
    HWND m_hBar;
    HWND m_hMin;
    HWND m_hNotify;

    RECT taskbarRect;
    RECT win11BridgeRect;
    RECT barRect;
    RECT minRect;
    RECT notifyRect;

    bool isWin11;

    double DPIScale;

    int DPI(int);
    void InitWindowPos();

public:
    taskbarWindow(
        const char* CLASS_NAME,
        const char* WINDOW_NAME,
        int width,
        HINSTANCE hInstance,
        int nCmdShow
    );
    ~taskbarWindow() {}

    HWND GetHwnd();
    void ShowWindow();
};