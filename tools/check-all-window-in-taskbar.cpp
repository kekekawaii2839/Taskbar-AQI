#include <Windows.h>
#include <iostream>
#include <vector>

#define DPI(x) int((x) * 1.25)

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    RECT rect;
    std::vector<char> className(256);
    GetClassName(hwnd, className.data(), className.size());
    GetWindowRect(hwnd, &rect);

    if (rect.left == rect.right or rect.top == rect.bottom) {
        return TRUE;
    }
    std::cout << "Class name: " << className.data() << std::endl;
    std::cout << "Window Rect: (" << DPI(rect.left) << ", " << DPI(rect.top) << ", " << DPI(rect.right) << ", " << DPI(rect.bottom) << ")" << std::endl;

    // check Z-index
    HWND hwndBelow = GetWindow(hwnd, GW_HWNDNEXT);
    HWND hwndAbove = GetWindow(hwnd, GW_HWNDPREV);
    std::vector<char> classNameBelow(256);
    std::vector<char> classNameAbove(256);
    GetClassName(hwndBelow, classNameBelow.data(), classNameBelow.size());
    GetClassName(hwndAbove, classNameAbove.data(), classNameAbove.size());
    std::cout << "Z-index: " << (hwndBelow ? "Below: " : "Bottommost") << classNameBelow.data() << ", " << (hwndAbove ? "Above: " : "Topmost") << classNameAbove.data() << std::endl;

    std::cout << "--------------------------------" << std::endl;
    
    return TRUE;
}

int main() {
    HWND m_hTaskbar = ::FindWindowW(L"Shell_TrayWnd", NULL);
    HWND m_hBar = ::FindWindowExW(m_hTaskbar, 0, L"ReBarWindow32", NULL);
    HWND m_hMin = ::FindWindowExW(m_hBar, 0, L"MSTaskSwWClass", NULL);
    EnumChildWindows(m_hTaskbar, EnumChildProc, 0);
    return 0;
}

/*
Class name: Windows.UI.Core.CoreWindow
Window Rect: (0, 1380, 1, 1381)
--------------------------------
Class name: Start
Window Rect: (0, 1380, 68, 1440)
--------------------------------
Class name: TrayNotifyWnd
Window Rect: (2191, 1380, 2560, 1440)
--------------------------------
Class name: InputIndicatorButton
Window Rect: (2191, 1380, 2236, 1440)
--------------------------------
Class name: ReBarWindow32
Window Rect: (68, 1380, 673, 1440)
--------------------------------
Class name: MSTaskSwWClass
Window Rect: (68, 1380, 673, 1440)
--------------------------------
Class name: MSTaskListWClass
Window Rect: (68, 1380, 673, 1440)
--------------------------------
Class name: Windows.UI.Composition.DesktopWindowContentBridge
Window Rect: (0, 1380, 2560, 1440)
--------------------------------
*/