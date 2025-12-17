#pragma once
#include <vector>
#include <map>

#include "Math.h"
#include "ShaderManager.h"
#include "Core.h"
#include "Window.h"
#include "Camera.h"
#include "Cube.h"
#include "CubeTextured.h"
#include "Duck.h"
#include "Grass.h"
#include "Light.h"
#include "Water.h"
#include "Enemy.h"
#include "Coin.h"
#include "Random.h"

#define DEFAULT_LIGTH "default_light"
#define WATER_LIGHT "water_light"
#define COIN_LIGHT "coin_light"
#define GRASS_LIGHT "grass_light"
#define LIGHT_WHEEL "light_wheel"

template <typename T>
void append(std::vector<T> &target, std::vector<T> &source) {
    target.insert(target.end(), source.begin(), source.end());
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

class Level1 {
public:
    Window *win;
    ShaderManager *sm;
    Core *core;
    Camera *camera;

    std::vector<Cube*> cubes;
    std::vector<CubeTextured*> cubesTextured;
    Duck *duck;
    Grass *grass;
    std::map<std::string, BRDFLightCB> lightsMap;
    Water *water;
    std::vector<Coin*> coins;
    std::vector<Enemy*> enemies;

    float timeAcc = 0.0f;

    Level1(Window *_win, ShaderManager *_sm, Core *_core, Camera *_camera) : win(_win), sm(_sm), core(_core), camera(_camera) {};

    void createLights() {
        BRDFLightCB light;
        light.lightColor = Vec3(1.0, 1.0, 1.0);
        light.lightDirection = Vec3(0.4, 1.0, 0.3);
        light.lightStrength = 3.0f;

        BRDFLightCB lightGrass = light;
        lightGrass.lightColor = Vec3(0.2, 1.0, 0.2);
        lightGrass.lightStrength = 5.0f;

        BRDFLightCB lightWater = light;
        lightWater.lightColor = Vec3(0.4, 0.4, 0.6);
        lightWater.lightStrength = 12.0f;

        BRDFLightCB lightCoin = light;
        lightCoin.lightColor = Vec3(1.0, 0.9, 0.9);
        lightCoin.lightStrength = 50.0f;

        BRDFLightCB lightWheel = light;
        lightWheel.lightStrength = 10.0f;

        lightsMap[DEFAULT_LIGTH] = light;
        lightsMap[GRASS_LIGHT] = lightGrass;
        lightsMap[WATER_LIGHT] = lightWater;
        lightsMap[COIN_LIGHT] = lightCoin;
        lightsMap[LIGHT_WHEEL] = lightWheel;
    }
    
    void createBlocksLayout() {
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
        generateCubesPositions(Vec3(-4, 6, -6), 3, 1, 4, lightDirtWithGrassPositions);
        generateCubesPositions(Vec3(-4, 6, 6), 3, 1, 2, lightDirtWithGrassPositions);
        generateCubesPositions(Vec3(-10, 6, -10), 4, 4, 1, lightDirtPositions);
        generateCubesPositions(Vec3(-10, 10, -8), 2, 1, 2, lightDirtPositions);
        generateCubesPositions(Vec3(-4, 10, -8), 1, 1, 1, lightDirtPositions);
        generateCubesPositions(Vec3(-8, 8, -8), 1, 2, 1, lightDirtPositions);
        generateCubesPositions(Vec3(-4, 12, -8), 1, 1, 2, lightDirtPositions);

        // grass stand
        generateCubesPositions(Vec3(0, 4, -10), 1, 1, 2, grassCubesPositions);

        generateGrassPositionsFromGrassCubes(grassCubesPositions, grassPositions, 50);
        generateGrassPositionsFromGrassCubes(lightDirtWithGrassPositions, grassPositions, 10);

        append(lightDirtPositions, lightDirtWithGrassPositions);

        Cube* grassCubes = Cube::createGrassCube(sm, core, grassCubesPositions);
        Cube* darkDirtCubes = Cube::createDarkDirtCube(sm, core, darkDirtPositions);
        CubeTextured* lightDirtCubes = CubeTextured::createBrickCubes(sm, core, lightDirtPositions, &lightsMap[DEFAULT_LIGTH]);
        
        Grass* _grass = Grass::createGrass(sm, core, grassPositions, &duck->vsCBAnimatedModel.W);

        cubes.push_back(grassCubes);
        cubes.push_back(darkDirtCubes);
        cubesTextured.push_back(lightDirtCubes);

        grass = _grass;
    }
    
    void createObjects() {
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

        Matrix waterWheelM1, rotationM1, scaleM1;
        rotationM1.setRotationY(90);
        scaleM1 = scaleM1.setScaling(Vec3(1.5,1.5,1.5));
        waterWheelM1 = waterWheelM1.setTranslation(Vec3(-4.0f, 6.0f, 3.0f)).mul(rotationM1).mul(scaleM1);
        std::vector<Matrix> waterWheelPos = {waterWheelM1};

        Matrix rail1, rail2, rail3, rail4;
        rail1 = rail1.setTranslation(Vec3(-1.5f, 4.0f, 8.2f)).mul(rail1.setScaling(Vec3(1.0, 1.2, 1.5)));
        rail2 = rail2.setTranslation(Vec3(1.5f, 4.0f, 8.2f)).mul(rail2.setScaling(Vec3(1.0, 1.2, 1.5)));
        rail3 = rail3.setTranslation(Vec3(-1.5f, 4.0f, -10.65f)).mul(rail3.setScaling(Vec3(1.0, 1.2, 1.5)));
        rail4 = rail4.setTranslation(Vec3(1.5f, 4.0f, -10.65f)).mul(rail4.setScaling(Vec3(1.0, 1.2, 1.5)));
        std::vector<Matrix> railsPos = {rail1, rail2, rail3, rail4};

        CubeTextured* pilar = new CubeTextured(sm, core, "models/pilar.gem");
        pilar->init(core, pilarPos, &lightsMap[DEFAULT_LIGTH], "models/textures/metal_color.png");

        CubeTextured* pilarBroken = new CubeTextured(sm, core, "models/pilar_broken.gem");
        pilarBroken->init(core, pilarBrokenPos, &lightsMap[DEFAULT_LIGTH], "models/textures/metal_color.png");

        CubeTextured* palmTree = new CubeTextured(sm, core, "models/palm_tree.gem");
        palmTree->init(core, palmTreePos, &lightsMap[DEFAULT_LIGTH], "models/textures/ColorPalette2.png");

        CubeTextured* rails = new CubeTextured(sm, core, "models/rail.gem");
        rails->init(core, railsPos, &lightsMap[DEFAULT_LIGTH], "models/textures/ColorPalette2.png");

        CubeTextured* waterWheel = new CubeTextured(sm, core, "models/waterwheel.gem");
        waterWheel->init(core, waterWheelPos, &lightsMap[LIGHT_WHEEL], "models/textures/ColorPalette.png");

        cubesTextured.push_back(pilar);
        cubesTextured.push_back(pilarBroken);
        cubesTextured.push_back(palmTree);
        cubesTextured.push_back(rails);
        cubesTextured.push_back(waterWheel);
    }

    void createWater() {
        Matrix waterPlaneM;
        waterPlaneM = waterPlaneM.setTranslation(Vec3(0.0f, 5.0f, -1.2f)).mul(waterPlaneM.setScaling(Vec3(1.0,1.0,0.85)));
        std::vector<Matrix> waterPlanePos = {waterPlaneM};

        Water *_water = Water::createWater(sm, core, waterPlanePos, &lightsMap[WATER_LIGHT]);
        water = _water;
    }

    void createCoins() {
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
        for (Matrix pos : coinsPos) {
            std::vector<Matrix> unary = {pos};
            Coin* _coin = Coin::createCoins(sm, core, unary, &lightsMap[COIN_LIGHT]);
            coins.push_back(_coin);
        }
    }

    void createEnemies() {
        Enemy *bull = new Enemy(sm, core, Vec3(-10.0f, 14.0f, 6.0f), Vec3(-4.0f, 14.0f, 6.0f), ALONG_X, -90, BULL_MODEL_FILE, 0.015, E_WALK_FORWARD);
        Enemy *bull2 = new Enemy(sm, core, Vec3(-4.0f, 14.0f, 1.0f), Vec3(-10.0f, 14.0f, 1.0f), ALONG_X, 90, BULL_MODEL_FILE, 0.015, E_WALK_FORWARD);
        Enemy *cat1 = new Enemy(sm, core, Vec3(-10.0f, 6.0f, -2.0f), Vec3(-10.0f, 6.0f, -2.0f), ALONG_Z, 180, CAT_SIAMESE_MODEL_FILE, 0.05, E_ATTACK);
        Enemy *cat2 = new Enemy(sm, core, Vec3(6.0f, 6.2f, 8.0f), Vec3(6.0f, 6.2f, 0.0f), ALONG_Z, 0, CAT_MODEL_FILE, 0.04, E_WALK_FORWARD, 0.15f);

        bull->setSize(Vec3(2.0, 2.0, 2.0));
        bull2->setSize(Vec3(2.0, 2.0, 2.0));
        cat1->setSize(Vec3(1.0, 1.0, 2.0));
        cat2->setSize(Vec3(1.0, 1.0, 2.0));

        enemies.push_back(bull);
        enemies.push_back(bull2);
        enemies.push_back(cat1);
        enemies.push_back(cat2);
    }

    void createDuck() {
        Duck *_duck = new Duck(sm, core, Vec3(8.0f, 16.0f, -3.0f), camera);
        duck = _duck;
    }

    void init() {
        createLights();
        createDuck();
        createBlocksLayout();
        createObjects();
        createWater();
        createCoins();
        createEnemies();
    }

    void checkRigidBodyCollision(GEMObject *object) {
        for (Matrix objectWorldMatrix : object->worldPositions) {
            bool isColidingX = duck->checkCollisionX(&objectWorldMatrix, object->size);
            if (isColidingX) {
                duck->blockMovementX();
            }
    
            bool isColidingY = duck->checkCollisionY(&objectWorldMatrix, object->size);
            if (isColidingY) {
                duck->isJumping = false;
                duck->jumpingCurrentHeight = 0.0f;
                duck->blockMovementY();
            }
    
            bool isColidingZ = duck->checkCollisionZ(&objectWorldMatrix, object->size);
            if (isColidingZ) {
                duck->blockMovementZ();
            }
        }
    }

    void checkCoinsCollision() {
        int idxColiding = -1;
        int idx = 0;
        for (Coin *coin : coins) {
            Matrix objectWorldMatrix = coin->worldPositions[0];
            bool isColidingX = duck->checkCollisionX(&objectWorldMatrix, coin->size);    
            bool isColidingY = duck->checkCollisionY(&objectWorldMatrix, coin->size);
            bool isColidingZ = duck->checkCollisionZ(&objectWorldMatrix, coin->size);
            if (isColidingX || isColidingY || isColidingZ) {
                idxColiding = idx;
                break;
            }
            idx++;
        }

        if (idxColiding != -1) {
            coins.erase(coins.begin() + idx);
        }
    }

    void checkEnemiesCollision() {
        if (duck->isDead) return;

        for (Enemy *enemy : enemies) {
            Matrix objectWorldMatrix = enemy->enemyModel.vertexShaderCB->W;
            bool isColidingX = duck->checkCollisionX(&objectWorldMatrix, enemy->size);    
            bool isColidingY = duck->checkCollisionY(&objectWorldMatrix, enemy->size);
            bool isColidingZ = duck->checkCollisionZ(&objectWorldMatrix, enemy->size);
            if (isColidingX || isColidingY || isColidingZ) {
                duck->isDead = true;
                break;
            }
        }
    }

    void checkCollisions() {
        for (Cube *cube : cubes) {
            checkRigidBodyCollision(cube);
        }

        for (CubeTextured *cubeTextured : cubesTextured) {
            checkRigidBodyCollision(cubeTextured);
        }

        checkCoinsCollision();
        checkEnemiesCollision();
    }

    void update(float dt) {
        timeAcc += dt;
        timeAcc = fmodf(timeAcc, 2 * 3.1415f); // Avoid precision issues

        duck->updateAnimation(win, dt);
        for (Enemy *enemy : enemies) {
            enemy->updateAnimation(win, dt);
        }

        checkCollisions();
    }

    void draw(float dt) {
        for (Cube *cube : cubes) {
            cube->draw(core, camera);
        }

        for (CubeTextured *cubeTextured : cubesTextured) {
            cubeTextured->draw(core, camera);
        }

        for (Coin *coin : coins) {
            coin->draw(core, camera, timeAcc);
        }
        
        for (Enemy *enemy : enemies) {
            enemy->draw(camera);
        }

        grass->draw(core, camera, &duck->vsCBAnimatedModel.W);
        duck->draw();

        water->draw(core, camera, dt);
    }
};