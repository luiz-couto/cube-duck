#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "GEMObject.h"

#define CUBE_MODEL "models/cube_2.gem"

struct CubePixelShaderCB {
    Vec3 topColor;
    Vec3 bottomColor;
    float heightThreshold;
    Vec3 lightDirection;
    float lightStrength;
};

class Cube : public GEMObject {
public:
    CubePixelShaderCB* pixelShaderCB;
    Cube(ShaderManager* sm, Core* core, const std::string& filename) : GEMObject(sm, core, filename) {}

    void init(Core* core, std::vector<Matrix> worldPositions, Vec3 topColor = Vec3(0.2, 1.0, 0.2), Vec3 bottomColor = Vec3(0.45, 0.2, 0.05), VertexDefaultShaderCB* vertexShader = nullptr, CubePixelShaderCB* pixelShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
        }
        vertexShaderCB = vertexShader;

        if (pixelShader == nullptr) {
            pixelShader = new CubePixelShaderCB();
            pixelShader->topColor = topColor;
            pixelShader->bottomColor = bottomColor;
            pixelShader->heightThreshold = 1.7f;
            pixelShader->lightDirection = Vec3(0.4, 1.0, 0.3);
            pixelShader->lightStrength = 0.7f;
        }
        pixelShaderCB = pixelShader;

        // Build geometry
        staticMesh.load(filename, worldPositions);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB);
        Shader* pixelShaderBlob = shaderManager->getPixelShader("shaders/pixel/CubePixelShader.hlsl", pixelShaderCB);
        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant("shaders/pixel/CubePixelShader.hlsl", "TopColor", &pixelShaderCB->topColor);
        shaderManager->updateConstant("shaders/pixel/CubePixelShader.hlsl", "BottomColor", &pixelShaderCB->bottomColor);
        shaderManager->updateConstant("shaders/pixel/CubePixelShader.hlsl", "HeightThreshold", &pixelShaderCB->heightThreshold);
        shaderManager->updateConstant("shaders/pixel/CubePixelShader.hlsl", "LightDirection", &pixelShaderCB->lightDirection);
        shaderManager->updateConstant("shaders/pixel/CubePixelShader.hlsl", "LightStrength", &pixelShaderCB->lightStrength);

        shaderManager->getPixelShader("shaders/pixel/CubePixelShader.hlsl", pixelShaderCB)->apply(core);
    }

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        updateConstantsVertexShader(core);
        updateConstantsPixelShader(core);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Cube* createGrassCube(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions) {
        Cube* cube = new Cube(sm, core, CUBE_MODEL);
        cube->init(core, worldPositions, Vec3(0.1, 0.6, 0.1), Vec3(0.45, 0.2, 0.05)); // green to brown
        return cube;
    }

    static Cube* createDarkDirtCube(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions) {
        Cube* cube = new Cube(sm, core, CUBE_MODEL);
        cube->init(core, worldPositions, Vec3(0.45, 0.2, 0.05), Vec3(0.45, 0.2, 0.05)); // all dark brown
        return cube;
    }

    static Cube* createLightDirtCube(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions) {
        Cube* cube = new Cube(sm, core, CUBE_MODEL);
        cube->init(core, worldPositions, Vec3(0.65, 0.35, 0.15), Vec3(0.65, 0.35, 0.15)); // all light brown
        return cube;
    }

    static Cube* createIceCube(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions) {
        Cube* cube = new Cube(sm, core, CUBE_MODEL);
        cube->init(core, worldPositions, Vec3(0.8, 0.9, 1.0), Vec3(0.5, 0.7, 0.9)); // light blue to darker blue
        return cube;
    }
};