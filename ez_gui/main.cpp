#include <iostream>

#include "widget_list.h"

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 1200

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    globals::widget_list_wndproc_handler(hwnd, message, w_param, l_param);
   
    return DefWindowProc(hwnd, message, w_param, l_param);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(NULL, L"WindowClass", L"renderer example", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    renderer renderer{};
    renderer.initialize(hwnd);
    widget::set_renderer(&renderer);

    MSG msg_;

    while (TRUE)
    {
        if (PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);

            if (msg_.message == WM_QUIT)
                break;
        }

        for (auto& widget_list : globals::widget_lists)
            widget_list.draw_widgets();

        renderer.draw();

        Sleep(10);
    }

    renderer.cleanup();
}

int main()
{

}