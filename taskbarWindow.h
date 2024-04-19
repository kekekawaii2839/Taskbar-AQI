#define WINVER 0x0605
#define DEBUG

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
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    ID2D1Factory* d2dFactory;
    IDWriteFactory* dwriteFactory;
    ID2D1HwndRenderTarget* renderTarget;

    int DPI(int);
    void InitWindowPos();
    void InitD2D1();

public:
    taskbarWindow(
        const char* CLASS_NAME,
        const char* WINDOW_NAME,
        int width,
        HINSTANCE hInstance,
        int nCmdShow
    );
    ~taskbarWindow();

    HWND GetHwnd();
    bool Paint();
    void ShowWindow();
};