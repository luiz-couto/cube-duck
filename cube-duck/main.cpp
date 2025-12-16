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
#include "Water.h"
#include "Enemy.h"
#include "Coin.h"

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

    Matrix palmTreeM, rotationM;
    rotationM.setRotationY(180);
    palmTreeM = palmTreeM.setTranslation(Vec3(2.0f, 5.7f, 7.5f)).mul(rotationM);
    std::vector<Matrix> palmTreePos = {palmTreeM};

    Matrix waterPlaneM;
    waterPlaneM = waterPlaneM.setTranslation(Vec3(0.0f, 5.0f, -1.2f)).mul(waterPlaneM.setScaling(Vec3(1.0,1.0,0.85)));
    std::vector<Matrix> waterPlanePos = {waterPlaneM};

    Matrix rail1, rail2, rail3, rail4;
    rail1 = rail1.setTranslation(Vec3(-1.5f, 4.0f, 8.2f)).mul(rail1.setScaling(Vec3(1.0, 1.2, 1.5)));
    rail2 = rail2.setTranslation(Vec3(1.5f, 4.0f, 8.2f)).mul(rail2.setScaling(Vec3(1.0, 1.2, 1.5)));
    rail3 = rail3.setTranslation(Vec3(-1.5f, 4.0f, -10.65f)).mul(rail3.setScaling(Vec3(1.0, 1.2, 1.5)));
    rail4 = rail4.setTranslation(Vec3(1.5f, 4.0f, -10.65f)).mul(rail4.setScaling(Vec3(1.0, 1.2, 1.5)));
    std::vector<Matrix> railsPos = {rail1, rail2, rail3, rail4};

    Matrix coin1, coin2, coin3, coin4, coin5, coin6, coin7, coin8;
    coin1 = coin1.setTranslation(Vec3(8.0f, 6.5f, 7.5f)).mul(coin1.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin2 = coin2.setTranslation(Vec3(0, 14, -3)).mul(coin2.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin3 = coin3.setTranslation(Vec3(2, 14, -3)).mul(coin3.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin4 = coin4.setTranslation(Vec3(-2, 14, -3)).mul(coin4.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin5 = coin5.setTranslation(Vec3(-4, 14, 8)).mul(coin5.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin6 = coin6.setTranslation(Vec3(-6, 14, 8)).mul(coin6.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin7 = coin7.setTranslation(Vec3(-8, 14, 8)).mul(coin7.setScaling(Vec3(0.015, 0.015, 0.015)));
    coin8 = coin8.setTranslation(Vec3(-10, 14, 8)).mul(coin8.setScaling(Vec3(0.015, 0.015, 0.015)));

    std::vector<Matrix> coinsPos = {coin1, coin2, coin3, coin4, coin5, coin6, coin7, coin8};

    append(lightDirtPositions, lightDirtWithGrassPositions);
    append(allCubesPositions, lightDirtPositions);
    append(allCubesPositions, darkDirtPositions);
    append(allCubesPositions, grassCubesPositions);
    append(allCubesPositions, pilarPos);
    append(allCubesPositions, pilarBrokenPos);
    append(allCubesPositions, palmTreePos);
    append(allCubesPositions, railsPos);

    BRDFLightCB light;
    light.lightColor = Vec3(1.0, 1.0, 1.0);
    light.lightDirection = Vec3(0.4, 1.0, 0.3);
    light.lightStrength = 3.0f;

    BRDFLightCB lightGrass = light;
    lightGrass.lightColor = Vec3(0.2, 1.0, 0.2);
    lightGrass.lightStrength = 5.0f;

    BRDFLightCB lightCoin = light;
    lightCoin.lightColor = Vec3(1.0, 0.9, 0.9);
    lightCoin.lightStrength = 50.0f;

    Cube* grassCubes = Cube::createGrassCube(shaderManager, &core, grassCubesPositions);
    CubeTextured* lightDirtCubes = CubeTextured::createBrickCubes(shaderManager, &core, lightDirtPositions, &light);
    Cube* darktDirtCubes = Cube::createDarkDirtCube(shaderManager, &core, darkDirtPositions);

    Duck duck(shaderManager, &core, Vec3(8.0f, 16.0f, -3.0f));
    Grass* grass = Grass::createGrass(shaderManager, &core, grassPositions, &duck.vsCBAnimatedModel.W);

    CubeTextured* pilar = new CubeTextured(shaderManager, &core, "models/pilar.gem");
    pilar->init(&core, pilarPos, &light, "models/textures/metal_color.png");

    CubeTextured* pilarBroken = new CubeTextured(shaderManager, &core, "models/pilar_broken.gem");
    pilarBroken->init(&core, pilarBrokenPos, &light, "models/textures/metal_color.png");

    CubeTextured* palmTree = new CubeTextured(shaderManager, &core, "models/palm_tree.gem");
    palmTree->init(&core, palmTreePos, &light, "models/textures/ColorPalette2.png");

    CubeTextured* rails = new CubeTextured(shaderManager, &core, "models/rail.gem");
    rails->init(&core, railsPos, &light, "models/textures/ColorPalette2.png");
    
    BRDFLightCB lightWater = light;
    lightWater.lightColor = Vec3(0.4, 0.4, 0.6);
    lightWater.lightStrength = 12.0f;

    Water *water = Water::createWater(shaderManager, &core, waterPlanePos, &lightWater);

    // enemies
    Enemy bull(shaderManager, &core, Vec3(-10.0f, 14.0f, 6.0f), Vec3(-4.0f, 14.0f, 6.0f), ALONG_X, -90, BULL_MODEL_FILE, 0.015, E_WALK_FORWARD);
    Enemy bull2(shaderManager, &core, Vec3(-4.0f, 14.0f, 1.0f), Vec3(-10.0f, 14.0f, 1.0f), ALONG_X, 90, BULL_MODEL_FILE, 0.015, E_WALK_FORWARD);
    Enemy cat1(shaderManager, &core, Vec3(-10.0f, 6.0f, -2.0f), Vec3(-10.0f, 6.0f, -2.0f), ALONG_Z, 180, CAT_SIAMESE_MODEL_FILE, 0.05, E_ATTACK);
    Enemy cat2(shaderManager, &core, Vec3(6.0f, 6.2f, 8.0f), Vec3(6.0f, 6.2f, 0.0f), ALONG_Z, 0, CAT_MODEL_FILE, 0.04, E_WALK_FORWARD, 0.15f);
    
    // coins
    Coin* coins = Coin::createCoins(shaderManager, &core, coinsPos, &lightCoin);

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
        palmTree->draw(&core, &camera);
        grassCubes->draw(&core, &camera);
        grass->draw(&core, &camera, &duck.vsCBAnimatedModel.W);
        rails->draw(&core, &camera);
        coins->draw(&core, &camera, time);
        
        duck.updateAnimation(&win, dt);
        bull.updateAnimation(&win, dt);
        bull2.updateAnimation(&win, dt);
        cat1.updateAnimation(&win, dt);
        cat2.updateAnimation(&win, dt);

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
        bull.draw(&camera);
        bull2.draw(&camera);
        cat1.draw(&camera);
        cat2.draw(&camera);

        water->draw(&core, &camera, dt);

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