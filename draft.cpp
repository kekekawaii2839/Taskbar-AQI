// #define DEBUG

#include "taskbarWindow.h"
#include "AQI.h"

#ifdef DEBUG
#include <iostream>
#endif

taskbarWindow* w;

wchar_t draw_str[256];
D2D1::ColorF brushColor(255, 255, 255, 1);

void DrawTextToScreen(HWND hwnd, const wchar_t* text, D2D1::ColorF brushColor) {
    ID2D1Factory* d2dFactory = nullptr;
    IDWriteFactory* dwriteFactory = nullptr;
    ID2D1HwndRenderTarget* renderTarget = nullptr;

    // init DirectWrite and Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));

    // create render target
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
    D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
    d2dFactory->CreateHwndRenderTarget(properties, D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY), &renderTarget);

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)); // transparent

    IDWriteTextFormat* textFormat;
    dwriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    ID2D1SolidColorBrush* brush;
    renderTarget->CreateSolidColorBrush(brushColor, &brush);

    renderTarget->DrawText(text, wcslen(text), textFormat, D2D1::RectF(0, 0, static_cast<float>(size.width), static_cast<float>(size.height)), brush);
    renderTarget->EndDraw();

    brush->Release();
    textFormat->Release();
    renderTarget->Release();
    dwriteFactory->Release();
    d2dFactory->Release();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            DrawTextToScreen(hwnd, draw_str, brushColor);
            return 0;
        // case WA_ACTIVE:
        //     DrawTextToScreen(hwnd, L"Hello, World!");
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

    AQI aqi("http://www.stateair.net/web/rss/1/1.xml");
    aqi.fetch_24_hours_data();
    int pm25 = aqi.get_latest_pm25();
    swprintf_s(draw_str, L"%d", pm25);
    brushColor = aqi.get_pm25_color(pm25);

    // main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
