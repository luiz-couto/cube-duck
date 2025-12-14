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
//#include "GrassLight.h"
#include "CubeTextured.h"
#include <random>

float generateRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(min, max);

    return dist(mt);
}

int generateRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);

    return dist(mt);
}

// Create Pipeline Manager to access many strcuts

void reactToCameraMovement(Window *win, Camera *camera, Duck *duck) {
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
        duck->resetPosition();
    }

    win->lastmousex = win->mousex;
    win->lastmousey = win->mousey;
}

void generateCubesPositions(Vec3 startPos, int numCubesY, int numCubesX, int numCubesZ, std::vector<Matrix> &cubesPositions) {
    for (int y = 0; y < numCubesY; y++ ) {
        for (int x = 0; x < numCubesX; x++) {
            for (int z = 0; z < numCubesZ; z++) {
                Matrix world, translation, rotation;
                float posX = startPos.x + (x * 1.95);
                float posY = startPos.y + (y * 1.95);
                float posZ = startPos.z + (z * 1.95);
                translation = translation.setTranslation(Vec3(posX, posY - 0.15, posZ));

                // SET RANDOM ROTAION BETWEEN 0, 90, 180, 270 DEGREES
                float possibleRotations[4] = {0.0f, 90.0f, 180.0f, 270.0f};
                int randomRotationIndex = static_cast<int>(generateRandomInt(0, 3));
                float rotationAngle = possibleRotations[randomRotationIndex];
                rotation.setRotationY(rotationAngle);
                
                world = translation.mul(rotation);
                cubesPositions.push_back(world);
            }
        }
    }
}

void generateGrassPositionsFromGrassCubes(std::vector<Matrix> &grassCubesPositions, std::vector<Matrix> &grassPositions, int factor) {
    for (const Matrix &cubeWorld : grassCubesPositions) {
        Matrix grassWorld;
        float i = cubeWorld.m[3] / 2;
        float y = cubeWorld.m[7] / 1.5f;
        float j = cubeWorld.m[11] / 2;

        for (int g = 0; g < factor; g++) {
            float minX = (i*2) - 0.92;
            float maxX = (i*2) + 0.92;

            float minZ = (j*2) - 0.92;
            float maxZ = (j*2) + 0.92;

            float x = generateRandomFloat(minX, maxX);
            float z = generateRandomFloat(minZ, maxZ);

            grassWorld = grassWorld.setTranslation(Vec3(x, (y * 1.5) + 2.0f, z)).mul(grassWorld.setScaling(Vec3(0.9,0.9,0.9)));
            grassPositions.push_back(grassWorld);
        }
    }
}

template <typename T>
void append(std::vector<T> &target, std::vector<T> &source) {
    target.insert(target.end(), source.begin(), source.end());
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

    std::vector<Matrix> lightDirtPositions = {};
    std::vector<Matrix> lightDirtWithGrassPositions = {};
    std::vector<Matrix> darkDirtPositions = {};
    std::vector<Matrix> grassCubesPositions = {};
    std::vector<Matrix> grassPositions = {};
    std::vector<Matrix> allCubesPositions = {};
    
    // base + right
    generateCubesPositions(Vec3(-10, 0, -10), 1, 10, 10, darkDirtPositions);
    generateCubesPositions(Vec3(-10, 2, -10), 1, 10, 10, lightDirtPositions);
    generateCubesPositions(Vec3(4, 4, -10), 1, 3, 10, grassCubesPositions);
    
    // pilar
    generateCubesPositions(Vec3(4, 6, -4), 3, 1, 2, lightDirtWithGrassPositions);
    generateCubesPositions(Vec3(8, 6, -4), 3, 1, 2, lightDirtWithGrassPositions);
    generateCubesPositions(Vec3(4, 12, -6), 1, 3, 4, lightDirtPositions);

    // bridge
    generateCubesPositions(Vec3(-10, 12, -4), 1, 7, 2, lightDirtPositions);

    // left base
    generateCubesPositions(Vec3(-10, 4, -10), 1, 4, 10, lightDirtPositions);
    generateCubesPositions(Vec3(-10, 6, 8), 4, 4, 1, lightDirtPositions);
    generateCubesPositions(Vec3(-10, 12, 0), 1, 4, 4, lightDirtPositions);

    // left tunnel
    generateCubesPositions(Vec3(-8, 6, -6), 2, 2, 6, lightDirtPositions);
    generateCubesPositions(Vec3(-4, 6, -6), 3, 1, 7, lightDirtWithGrassPositions);
    generateCubesPositions(Vec3(-10, 6, -10), 4, 4, 1, lightDirtPositions);
    generateCubesPositions(Vec3(-10, 10, -8), 2, 1, 2, lightDirtPositions);
    generateCubesPositions(Vec3(-4, 10, -8), 1, 1, 1, lightDirtPositions);
    generateCubesPositions(Vec3(-8, 8, -8), 1, 2, 1, lightDirtPositions);
    generateCubesPositions(Vec3(-4, 12, -8), 1, 1, 2, lightDirtPositions);

    // grass stand
    generateCubesPositions(Vec3(0, 4, -10), 1, 1, 2, grassCubesPositions);

    generateGrassPositionsFromGrassCubes(grassCubesPositions, grassPositions, 50);
    generateGrassPositionsFromGrassCubes(lightDirtWithGrassPositions, grassPositions, 10);

    Matrix pilar1;
    pilar1 = pilar1.setTranslation(Vec3(4.0f, 13.7f, -6.0f)).mul(pilar1.setScaling(Vec3(0.6,0.6,0.6)));
    std::vector<Matrix> pilarPos = {pilar1};

    Matrix pilarBrokenM;
    pilarBrokenM = pilarBrokenM.setTranslation(Vec3(4.0f, 13.7f, 0.0f)).mul(pilarBrokenM.setScaling(Vec3(0.6,0.6,0.6)));
    std::vector<Matrix> pilarBrokenPos = {pilarBrokenM};

    append(lightDirtPositions, lightDirtWithGrassPositions);
    append(allCubesPositions, lightDirtPositions);
    append(allCubesPositions, darkDirtPositions);
    append(allCubesPositions, grassCubesPositions);
    append(allCubesPositions, pilarPos);
    append(allCubesPositions, pilarBrokenPos);

    BRDFLightCB light;
    light.lightColor = Vec3(1.0, 1.0, 1.0);
    light.lightDirection = Vec3(0.4, 1.0, 0.3);
    light.lightStrength = 3.0f;

    BRDFLightCB lightGrass = light;
    lightGrass.lightColor = Vec3(0.2, 1.0, 0.2);
    lightGrass.lightStrength = 5.0f;

    Cube* grassCubes = Cube::createGrassCube(shaderManager, &core, grassCubesPositions);
    CubeTextured* lightDirtCubes = CubeTextured::createBrickCubes(shaderManager, &core, lightDirtPositions, &light);
    Cube* darktDirtCubes = Cube::createDarkDirtCube(shaderManager, &core, darkDirtPositions);

    Duck duck(shaderManager, &core, Vec3(8.0f, 16.0f, -3.0f));
    Grass* grass = Grass::createGrass(shaderManager, &core, grassPositions, &duck.vsCBAnimatedModel.W);

    CubeTextured* pilar = new CubeTextured(shaderManager, &core, "models/pilar.gem");
    pilar->init(&core, pilarPos, &light, "models/textures/metal_color.png");

    CubeTextured* pilarBroken = new CubeTextured(shaderManager, &core, "models/pilar_broken.gem");
    pilarBroken->init(&core, pilarBrokenPos, &light, "models/textures/metal_color.png");
    
    GamesEngineeringBase::Timer tim = GamesEngineeringBase::Timer();
    float time = 0.0f;

    while (true) {
        core.beginFrame();
        win.processMessages();
        if (win.keys[VK_ESCAPE] == 1) {
            break;
        }

        reactToCameraMovement(&win, &camera, &duck);
        
        float dt = tim.dt();
        time += dt;
        time = fmodf(time, 2 * 3.1415f); // Avoid precision issues

        // Vec3 from = Vec3(11 * cos(time), 5, 11 * sinf(time));
        // camera.from = from;

        core.beginRenderPass();

        sky.draw(&core, &camera, dt);

        darktDirtCubes->draw(&core, &camera);
        lightDirtCubes->draw(&core, &camera);
        pilar->draw(&core, &camera);
        pilarBroken->draw(&core, &camera);
        grassCubes->draw(&core, &camera);
        grass->draw(&core, &camera, &duck.vsCBAnimatedModel.W);

        duck.updateAnimation(&win, dt);

        for (auto cubeWorldMatrix : allCubesPositions) {
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