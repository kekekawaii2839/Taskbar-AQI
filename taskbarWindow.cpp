#include "taskbarWindow.h"

taskbarWindow::taskbarWindow(
    const char* CLASS_NAME,
    const char* WINDOW_NAME,
    int width,
    HINSTANCE hInstance,
    int nCmdShow
) {
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
        ::MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
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
        ::MessageBox(NULL, "Insert to taskbar failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
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

HWND taskbarWindow::GetHwnd() {
    return hwnd;
}

void taskbarWindow::ShowWindow() {
    ::ShowWindow(hwnd, nCmdshow);
    // Paint();
    // ::UpdateWindow(hwnd);
}