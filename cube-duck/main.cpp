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
#include "SkyDome.h"
#include "Grass.h"
#include <random>

float generateRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(min, max);

    return dist(mt);
}

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

    SkyDome sky(shaderManager);
    sky.init(&core);

    std::vector<Matrix> worldPositions = {};
    std::vector<Matrix> grassPositions = {};

    int radius = 5;
    for (int y = -2; y < 2; y++) {
        for (int i = -radius; i < radius; i++) {
            for (int j= -radius; j < radius; j++) {
                Matrix world, grassWorld;
                world = world.setTranslation(Vec3(i * 2, y * 1.5, j * 2));

                for (int g = 0; g < 30; g++) {
                    float minX = (i*2) - 0.9;
                    float maxX = (i*2) + 0.9;

                    float minZ = (j*2) - 0.9;
                    float maxZ = (j*2) + 0.9;

                    float x = generateRandomFloat(minX, maxX);
                    float z = generateRandomFloat(minZ, maxZ);

                    grassWorld = grassWorld.setTranslation(Vec3(x, (y * 1.5) + 2.0f, z)).mul(grassWorld.setScaling(Vec3(0.9,0.9,0.9)));
                    grassPositions.push_back(grassWorld);

                }
                
                worldPositions.push_back(world);
            }
        }
        radius -= 2;
    }

    Cube* cubes = Cube::createGrassCube(shaderManager, &core, worldPositions);
    Duck duck(shaderManager, &core, Vec3(3.0f, 5.0f, 1.0f));
    Grass* grass = Grass::createGrass(shaderManager, &core, grassPositions, &duck.vsCBAnimatedModel.W);

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

        sky.draw(&core, &camera, dt);

        cubes->draw(&core, &camera);
        grass->draw(&core, &camera, &duck.vsCBAnimatedModel.W);

        duck.updateAnimation(&win, dt);

        for (auto cubeWorldMatrix : worldPositions) {
            bool isColidingX = duck.checkCollisionX(&cubeWorldMatrix, 2);
            if (isColidingX) {
                duck.blockMovementX();
            }

            bool isColidingY = duck.checkCollisionY(&cubeWorldMatrix, 2);
            if (isColidingY) {
                duck.isJumping = false;
                duck.jumpingCurrentHeight = 0.0f;
                duck.blockMovementY();
            }

            bool isColidingZ = duck.checkCollisionZ(&cubeWorldMatrix, 2);
            if (isColidingZ) {
                duck.blockMovementZ();
            }
        }

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