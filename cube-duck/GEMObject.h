#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"

struct CubePixelShaderCB {
    Vec3 topColor;
    Vec3 bottomColor;
    float heightThreshold;
    Vec3 lightDirection;
    float lightStrength;
};

class GEMObject {
public:
    StaticMesh staticMesh;
    ShaderManager* shaderManager;
    PSOManager psos;
    VertexLayoutCache vertexLayoutCache;
    std::string filename;
    VertexDefaultShaderCB* vertexShaderCB;
    CubePixelShaderCB* pixelShaderCB;

    GEMObject(ShaderManager* sm, Core* core, const std::string& filename) : shaderManager(sm), staticMesh(core), filename(filename) {}

    void init(Core* core, VertexDefaultShaderCB* vertexShader = nullptr, CubePixelShaderCB* pixelShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
        }
        vertexShaderCB = vertexShader;

        if (pixelShader == nullptr) {
            pixelShader = new CubePixelShaderCB();
            pixelShader->topColor = Vec3(0.2, 1.0, 0.2);           // green
            pixelShader->bottomColor = Vec3(0.45, 0.2, 0.05);      // brown
            pixelShader->heightThreshold = 1.45f;
            pixelShader->lightDirection = Vec3(0.4, 1.0, 0.3);
            pixelShader->lightStrength = 0.7f;
        }
        pixelShaderCB = pixelShader;

        // Build geometry
        staticMesh.load(filename);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB);
        Shader* pixelShaderBlob = shaderManager->getPixelShader("shaders/pixel/CubePixelShader.hlsl", pixelShaderCB);
        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void scale(float s) {
        vertexShaderCB->W = vertexShaderCB->W.setScaling(Vec3(s, s, s));
    }

    void translate(const Vec3& t) {
        vertexShaderCB->W = vertexShaderCB->W.setTranslation(t);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB)->apply(core);
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
};