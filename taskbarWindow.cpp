#include "taskbarWindow.h"

taskbarWindow::taskbarWindow(
    const char* CLASS_NAME,
    const char* WINDOW_NAME,
    int width,
    HINSTANCE hInstance,
    int nCmdShow
) {
    #ifdef DEBUG
    std::cout << "taskbarWindow constructor!" << std::endl;
    #endif
    this->nCmdshow = nCmdShow;
    this->width = width;

    hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        CLASS_NAME, WINDOW_NAME,
        WS_POPUP | WS_VISIBLE | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT, width, 100,
        NULL, NULL, hInstance, NULL
    );
    #ifdef DEBUG
    std::cout << "hwnd: " << hwnd << std::endl;
    #endif

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    #ifdef DEBUG
    std::cout << "GdiplusStartup! " << std::endl;
    #endif

    m_hTaskbar = ::FindWindowW(L"Shell_TrayWnd", NULL);
    m_hWin11Bridge = ::FindWindowExW(m_hTaskbar, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
    m_hBar = ::FindWindowExW(m_hTaskbar, NULL, L"ReBarWindow32", NULL);
    m_hMin = ::FindWindowExW(m_hBar, NULL, L"MSTaskSwWClass", NULL);
    m_hNotify = ::FindWindowExW(m_hTaskbar, NULL, L"TrayNotifyWnd", NULL);

    isWin11 = (m_hWin11Bridge != NULL);

    DPIScale = ::GetDpiForWindow(m_hTaskbar) / 96.0;

    InitWindowPos();
    InitD2D1();
}

taskbarWindow::~taskbarWindow() {
    Gdiplus::GdiplusShutdown(gdiplusToken);
    renderTarget->Release();
    dwriteFactory->Release();
    d2dFactory->Release();
}

int taskbarWindow::DPI(int x) {
    return int(x * DPIScale);
}

void taskbarWindow::InitWindowPos() {
    ::GetWindowRect(m_hTaskbar, &taskbarRect);
    ::GetWindowRect(m_hWin11Bridge, &win11BridgeRect);
    ::GetWindowRect(m_hBar, &barRect);
    ::GetWindowRect(m_hMin, &minRect);
    ::GetWindowRect(m_hNotify, &notifyRect);

    bool isInsertToTaskbar = ::SetParent(hwnd, isWin11 ? m_hTaskbar : m_hBar);
    if (!isInsertToTaskbar) {
        MessageBox(NULL, "Insert to taskbar failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    ::MoveWindow(hwnd,
        DPI(notifyRect.left - width - 10 - taskbarRect.left),
        notifyRect.top - taskbarRect.top,
        DPI(width),
        DPI(notifyRect.bottom - notifyRect.top),
        TRUE
    );

    ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ::SetWindowLongW(hwnd, GWL_EXSTYLE, 0x00000000L | 0x00010000L | 0x00080000 | 0x02000000L | 0x00000020L);
    ::SetLayeredWindowAttributes(hwnd, 0, 255, 0x00000001 | 0x00000002);
}

void taskbarWindow::InitD2D1() {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));
}

HWND taskbarWindow::GetHwnd() {
    return hwnd;
}

void taskbarWindow::ShowWindow() {
    ::ShowWindow(hwnd, nCmdshow);
    Paint();
    ::UpdateWindow(hwnd);
}

bool taskbarWindow::Paint() {
    const wchar_t* text = L"Hello, World!";

    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
    D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
    d2dFactory->CreateHwndRenderTarget(properties, D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY), &renderTarget);

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    IDWriteTextFormat* textFormat;
    dwriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    ID2D1SolidColorBrush* brush;
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);

    renderTarget->DrawText(text, wcslen(text), textFormat, D2D1::RectF(0, 0, static_cast<float>(size.width), static_cast<float>(size.height)), brush);
    renderTarget->EndDraw();

    brush->Release();
    textFormat->Release();
}