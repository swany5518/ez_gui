#include <iostream>

#include "widget_list.h"

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 1200

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    globals::widget_list_wndproc_handler(hwnd, message, w_param, l_param);
   
    if (message == WM_CLOSE)
        std::cout << std::endl;

    if (message == WM_DESTROY)
        ::PostQuitMessage(0);
    
    return DefWindowProc(hwnd, message, w_param, l_param);
}

int main()
{
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";
    auto rc_atom = RegisterClassExW(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(NULL, L"WindowClass", L"renderer example", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, GetModuleHandle(NULL), NULL);
    ShowWindow(hwnd, SW_SHOW);

    renderer renderer{};
    renderer.initialize(hwnd);
    renderer.set_render_target_color(colors::white);
    widget::set_renderer(&renderer);

    slider_style sldr_style_test{ text_style{12.f, colors::blue}, border_style{1.f, colors::red}, mc_rect{colors::black}, mc_rect{colors::gray} };
    float test_float = 20.f;
    slider<float> slider_test{ vec2{500.f, 100.f}, vec2{100.f, 20.f}, L"test slider", &test_float, 0.f, 100.f, &sldr_style_test };

    color test_clr{0.f, 0.f, 1.f, 1.f};
    color_editor_style clr_edit_style{ {}, {}, {colors::gray} };
    color_editor editor{ {400.f, 500.f}, {300.f, 200.f}, L"editor", &test_clr, &clr_edit_style };

    globals::widget_lists.emplace_back(std::move(get_slider_style_edit_list(&sldr_style_test)));
    globals::widget_lists[0].add_widget(&slider_test);
    globals::widget_lists[0].add_widget(&editor);

    MSG msg;
    while (TRUE)
    {
        ZeroMemory(&msg, sizeof(msg));
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (auto& widget_list : globals::widget_lists)
            widget_list.draw_widgets();

        if (GetAsyncKeyState(VK_INSERT) & 0x1)
            globals::widget_lists.front().to_string();

        renderer.draw();

        Sleep(10);
    }

    renderer.cleanup();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    main();
}