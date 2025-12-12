#include "Window.h"
#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "GamesEngineeringBase.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "GEMObject.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Cube.h"
#include "Duck.h"

// Create Pipeline Manager to access many strcuts

void reactToCameraMovement(Window *win, Camera *camera) {
    float mouseOffsetY = win->lastmousey - win->mousey;
    float mouseOffsetX = win->lastmousex - win->mousex;

    if (!win->mouseButtons[0] && std::abs(mouseOffsetY) > 0.00001f) {
        camera->moveCameraY(mouseOffsetY * 0.01f);
    }

    if (win->mouseButtons[0] && std::abs(mouseOffsetX) > 0.00001f) {
        camera->rotate(-mouseOffsetX * 0.01f);
    } else if (std::abs(mouseOffsetX) > 0.00001f) {
        camera->moveCameraX(-mouseOffsetX * 0.01f);
    }

    if (!win->mouseButtons[0] && win->mouseWheelDelta != 0) {
        camera->zoom(win->mouseWheelDelta * 0.01f);
        win->mouseWheelDelta = 0;
    }

    if (win->keys['R']) {
        camera->resetCamera();
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

    std::vector<Matrix> worldPositions = {};
    for (int y = -5; y < 5; y++) {
        for (int i = -5; i < 5; i++) {
            for (int j=-5; j<5; j++) {
                Matrix world;
                world = world.setTranslation(Vec3(i * 2, y * 1.5, j * 2));
                worldPositions.push_back(world);
            }
        }
    }

    Cube* cube = Cube::createGrassCube(shaderManager, &core, worldPositions);
    Duck duck(shaderManager, &core);

    GamesEngineeringBase::Timer tim = GamesEngineeringBase::Timer();
    float time = 0.0f;

    while (true) {
        core.beginFrame();
        win.processMessages();
        if (win.keys[VK_ESCAPE] == 1) {
            break;
        }

        reactToCameraMovement(&win, &camera);
        
        float dt = tim.dt();
        time += dt;
        time = fmodf(time, 2 * 3.1415f); // Avoid precision issues

        // Vec3 from = Vec3(11 * cos(time), 5, 11 * sinf(time));
        // camera.from = from;

        core.beginRenderPass();

        cube->draw(&core, &camera);

        duck.updateAnimation(dt);
        duck.draw(&camera);

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