#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "GEMObject.h"
#include "Texture.h"
#include "Light.h"

struct BrickVertexShaderCB {
    Matrix W;
    Matrix VP;
};

class Brick : public GEMObject {
public:
    BRDFLightCB* lightCB;
    BrickVertexShaderCB* vertexShaderCB;
    Texture *texture;
    Brick(ShaderManager* sm, Core* core, const std::string& filename) : GEMObject(sm, core, filename) {}

    void init(Core* core, std::vector<Matrix> worldPositions, BRDFLightCB* light) {
        BrickVertexShaderCB* vertexShader = new BrickVertexShaderCB();
        vertexShader->W.setIdentity();
        vertexShader->VP.setIdentity();
        
        vertexShaderCB = vertexShader;

        lightCB = light;

        // load texture
        texture = new Texture();
        texture->load(core, "models/textures/ColorPalette2.png");

        // Build geometry
        staticMesh.load(filename, worldPositions);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/BrickVertexShader.hlsl", vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader("shaders/pixel/PixelShaderLightTexture.hlsl", lightCB);

        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant("shaders/vertex/BrickVertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/BrickVertexShader.hlsl", "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader("shaders/vertex/BrickVertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant("shaders/pixel/PixelShaderLightTexture.hlsl", "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant("shaders/pixel/PixelShaderLightTexture.hlsl", "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant("shaders/pixel/PixelShaderLightTexture.hlsl", "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader("shaders/pixel/PixelShaderLightTexture.hlsl", lightCB)->apply(core);
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        updateConstantsVertexShader(core);
        updateConstantsPixelShader(core);
        shaderManager->updateTexturePS(core, "Brick", texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Brick* createBrick(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
Brick* brick = new Brick(sm, core, "models/brick_2.gem");
        brick->init(core, worldPositions, light);
        return brick;
    }
};