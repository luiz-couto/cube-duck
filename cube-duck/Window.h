#pragma once

#define NORINMAX
#include <Windows.h>
#include <string>

#define WINDOW_HEIGHT 728
#define WINDOW_WIDTH 1024

#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class Window {
public:
    HWND hwnd;
    HINSTANCE hinstance;
    int windowWidth, windowHeight;
    std::string windowName;

    bool keys[256];
    int mousex = WINDOW_WIDTH / 2;
    int mousey = WINDOW_HEIGHT / 2;
    bool mouseButtons[3];

    int lastmousex = WINDOW_WIDTH / 2;
    int lastmousey = WINDOW_HEIGHT / 2;
    int mouseWheelDelta = 0;

    Window();
    void init(int _windowHeight, int _windowWidth, int window_x, int window_y, std::string _windowName);

    void updateMouse(int x, int y) {
        mousex = x;
        mousey = y;
    }

    void processMessages();
};