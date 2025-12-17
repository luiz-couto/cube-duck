#include "Window.h"
#include "Core.h"
#include "GamesEngineeringBase.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "SkyDome.h"
#include "Level1.h"

void reactToCameraMovement(Window *win, Camera *camera, Duck *duck) {
    float mouseOffsetY = win->lastmousey - win->mousey;
    float mouseOffsetX = win->lastmousex - win->mousex;

    if (win->mouseButtons[0] && std::abs(mouseOffsetY) > 0.00001f) {
        camera->rotateZ(-mouseOffsetY * 0.01);
    } else if (std::abs(mouseOffsetY) > 0.00001f) {
        camera->moveCameraY(mouseOffsetY * 0.01f);
    }

    if (win->mouseButtons[0] && std::abs(mouseOffsetX) > 0.00001f) {
        camera->rotateY(-mouseOffsetX * 0.01f);
    } else if (std::abs(mouseOffsetX) > 0.00001f) {
        camera->moveCameraX(-mouseOffsetX * 0.01f);
    }

    if (!win->mouseButtons[0] && win->mouseWheelDelta != 0) {
        camera->zoom(win->mouseWheelDelta * 0.01f);
        win->mouseWheelDelta = 0;
    }

    if (win->keys['R']) {
        camera->resetCamera();
        duck->resetPosition();
    }

    win->lastmousex = win->mousex;
    win->lastmousey = win->mousey;
}

void mainLoop() {
    Window win;
    win.init(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0, "My Window");

    Core core;
    core.init(win.hwnd, win.windowWidth, win.windowHeight);
    
    ShaderManager* shaderManager = new ShaderManager(&core);
    Camera camera;

    SkyDome sky(shaderManager);
    sky.init(&core);

    Level1 level1(&win, shaderManager, &core, &camera);
    level1.init();

    GamesEngineeringBase::Timer tim = GamesEngineeringBase::Timer();

    while (true) {
        core.beginFrame();
        win.processMessages();
        if (win.keys[VK_ESCAPE] == 1) {
            break;
        }

        reactToCameraMovement(&win, &camera, level1.duck);

        float dt = tim.dt();

        core.beginRenderPass();
        sky.draw(&core, &camera, dt);

        level1.update(dt);
        level1.draw(dt);

        core.finishFrame();
    }

    core.flushGraphicsQueue();
}

// int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
//     mainLoop();
//     return 0;
// }

int main() {
    mainLoop();
    return 0;
}