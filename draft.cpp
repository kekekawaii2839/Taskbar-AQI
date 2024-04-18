#define WINVER 0x0605
// #define DEBUG

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

using namespace Gdiplus;

double scale;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

#define DPI(x) int((x) * scale)

void PrintRect(RECT rect, const char* name = "") {
    #ifdef DEBUG
    std::cout << name << ": (" << DPI(rect.left) << ", " << DPI(rect.top) << ", " << DPI(rect.right) << ", " << DPI(rect.bottom) << ")" << std::endl;
    #endif
}

void DrawTextToScreen(HWND hwnd, const wchar_t* text) {
    ID2D1Factory* d2dFactory = nullptr;
    IDWriteFactory* dwriteFactory = nullptr;
    ID2D1HwndRenderTarget* renderTarget = nullptr;

    // 初始化 DirectWrite 和 Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));

    // 创建渲染目标
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
    D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
    d2dFactory->CreateHwndRenderTarget(properties, D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY), &renderTarget);

    // 清空背景色
    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)); // 透明色

    // 创建文本格式
    IDWriteTextFormat* textFormat;
    dwriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    // 创建画刷
    ID2D1SolidColorBrush* brush;
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);

    // 绘制文本
    renderTarget->DrawText(text, wcslen(text), textFormat, D2D1::RectF(0, 0, static_cast<float>(size.width), static_cast<float>(size.height)), brush);

    // 结束绘制
    renderTarget->EndDraw();

    // 释放资源
    brush->Release();
    textFormat->Release();
    renderTarget->Release();
    dwriteFactory->Release();
    d2dFactory->Release();
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            GdiplusShutdown(gdiplusToken);
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            DrawTextToScreen(hwnd, L"Hello, World!");
            return 0;
        case WA_ACTIVE:
            DrawTextToScreen(hwnd, L"Hello, World!");
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 定义窗口类
    const char* CLASS_NAME = "MyGO!!!!!";

    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        CLASS_NAME,
        "MyGO!!!!!",
        WS_POPUP | WS_VISIBLE | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HWND m_hTaskbar = ::FindWindowW(L"Shell_TrayWnd", NULL);
    HWND m_hWin11Bridge = ::FindWindowExW(m_hTaskbar, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
    bool isWin11 = (m_hWin11Bridge != NULL);
    HWND m_hBar = ::FindWindowExW(m_hTaskbar, 0, L"ReBarWindow32", NULL);
    HWND m_hMin = ::FindWindowExW(m_hBar, 0, L"MSTaskSwWClass", NULL);
    HWND m_hNotify = ::FindWindowExW(m_hTaskbar, 0, L"TrayNotifyWnd", NULL);

    UINT DPI = ::GetDpiForWindow(m_hTaskbar);
    scale = DPI / 96.0;
    #ifdef DEBUG
    std::cout << "scale: " << scale << std::endl;
    #endif

    RECT taskbarRect = {0};
    RECT win11BridgeRect = {0};
    RECT barRect = {0};
    RECT minRect = {0};
    RECT notifyRect = {0};
    RECT hwndRect = {0};

    ::GetWindowRect(m_hTaskbar, &taskbarRect);
    ::GetWindowRect(m_hWin11Bridge, &win11BridgeRect);
    ::GetWindowRect(m_hBar, &barRect);
    ::GetWindowRect(m_hMin, &minRect);
    ::GetWindowRect(m_hNotify, &notifyRect);

    #ifdef DEBUG
    PrintRect(taskbarRect, "taskbarRect");
    PrintRect(win11BridgeRect, "win11BridgeRect");
    PrintRect(barRect, "barRect");
    PrintRect(minRect, "minRect");
    PrintRect(notifyRect, "notifyRect");
    std::cout << "---------------------------------------------" << std::endl;
    #endif

    // std::cout << "minRect: " << minRect.left << ", " << minRect.top << ", " << minRect.right << ", " << minRect.bottom << std::endl;
    // std::cout << ::MoveWindow(m_hMin, 0, 0, minRect.right - minRect.left - 200, minRect.bottom - minRect.top, TRUE) << std::endl;
    // GetWindowRect(m_hMin, &minRect);
    // std::cout << "minRect: " << minRect.left << ", " << minRect.top << ", " << minRect.right << ", " << minRect.bottom << std::endl;

    #ifdef DEBUG
    std::cout << "isWin11: " << isWin11 << std::endl;
    #endif

    bool isInsertToTaskbar = ::SetParent(hwnd, isWin11 ? m_hTaskbar : m_hBar);
    #ifdef DEBUG
    std::cout << "isInsertToTaskbar: " << isInsertToTaskbar << std::endl;
    #endif
    ::GetWindowRect(hwnd, &hwndRect);
    PrintRect(hwndRect, "hwndRect");

    ::MoveWindow(hwnd,
        DPI(notifyRect.left - 200 - taskbarRect.left),
        notifyRect.top - taskbarRect.top,
        DPI(190),
        DPI(notifyRect.bottom - notifyRect.top),
        TRUE
    );
    ::GetWindowRect(hwnd, &hwndRect);
    PrintRect(hwndRect, "hwndRect");
    ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ::SetWindowLongW(hwnd, GWL_EXSTYLE, 0x00000000L | 0x00010000L | 0x00080000 | 0x02000000L | 0x00000020L);
    ::SetLayeredWindowAttributes(hwnd, 0, 255, 0x00000001 | 0x00000002);

    ::ShowWindow(hwnd, nCmdShow);

    // main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
