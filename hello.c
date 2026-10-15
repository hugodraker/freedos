/* =====================================================================
 * MicroWin - Win16 "Hello World" Window Demo
 * Author: Hugo
 * Date: October 12, 2026
 * Released to the Public Domain
 *
 * COMPILATION INSTRUCTIONS (OpenWatcom):
 *   remove all #include <windows.h>, and simular .h, replace with #include "microwin.h"
 *   compile for example
 *   wcl -bt=windows -ml -os hello.c
 * ===================================================================== */
#include "microwin.h"

#define ID_BTN_CLOSE 101

/* Window Procedure */
LRESULT CALLBACK __export WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindow(
                "BUTTON",
                "Close",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                95, 80, 70, 24,
                hwnd,
                (HMENU)ID_BTN_CLOSE,
                (HINSTANCE)GetWindowWord(hwnd, GWW_HINSTANCE),
                NULL
            );
            return 0;
        }

        case WM_COMMAND: {
            if (wParam == ID_BTN_CLOSE) {
                DestroyWindow(hwnd);
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            RECT rc;
            HDC hdc = BeginPaint(hwnd, &ps);
            HFONT hFont = CreateFont(
                16, 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE,
                ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                FF_SWISS, "Arial"
            );
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            GetClientRect(hwnd, &rc);
            rc.bottom = 60;

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);

            DrawText(hdc, "Hello, World!", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

/* Win16 Entry Point */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    (void)lpCmdLine;

    if (!hPrevInstance) {
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = "HelloDemoClass";

        if (!RegisterClass(&wc)) {
            return FALSE;
        }
    }

    hwnd = CreateWindow(
        "HelloDemoClass",
        "Hello World Demo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        180, 150, 260, 130,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}