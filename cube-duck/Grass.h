#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "GEMObject.h"
#include "Texture.h"

struct GrassVertexShaderCB {
    Matrix W;
    Matrix VP;
    Matrix PLAYER_POS;
};

struct GrassPixelShaderCB {
    Vec3 topColor;
    Vec3 bottomColor;
    float heightThreshold;
    Vec3 lightDirection;
    float lightStrength;
};

class Grass : public GEMObject {
public:
    GrassPixelShaderCB* pixelShaderCB;
    GrassVertexShaderCB* vertexShaderCB;
    Texture *texture;
    Grass(ShaderManager* sm, Core* core, const std::string& filename) : GEMObject(sm, core, filename) {}

    void init(Core* core, std::vector<Matrix> worldPositions, Matrix *playerPos, GrassVertexShaderCB* vertexShader = nullptr, GrassPixelShaderCB* pixelShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new GrassVertexShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
            vertexShader->PLAYER_POS = *playerPos;
        }
        vertexShaderCB = vertexShader;

        // if (pixelShader == nullptr) {
        //     pixelShader = new GrassPixelShaderCB();
        //     pixelShader->topColor = topColor;
        //     pixelShader->bottomColor = bottomColor;
        //     pixelShader->heightThreshold = 1.45f;
        //     pixelShader->lightDirection = Vec3(0.4, 1.0, 0.3);
        //     pixelShader->lightStrength = 0.7f;
        // }
        // pixelShaderCB = pixelShader;

        // load texture
        texture = new Texture();
        texture->load(core, "models/textures/Grass.png");

        // Build geometry
        staticMesh.load(filename, worldPositions);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/GrassVertexShader.hlsl", vertexShaderCB);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShaderTexture.hlsl", PIXEL_SHADER);

        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant("shaders/vertex/GrassVertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/GrassVertexShader.hlsl", "VP", &vertexShaderCB->VP);
        shaderManager->updateConstant("shaders/vertex/GrassVertexShader.hlsl", "PLAYER_POS", &vertexShaderCB->PLAYER_POS);

        shaderManager->getVertexShader("shaders/vertex/GrassVertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void draw(Core* core, Camera* camera, Matrix *playerPos) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        vertexShaderCB->PLAYER_POS = *playerPos;
        updateConstantsVertexShader(core);

        shaderManager->updateTexturePS(core, "Grass", texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Grass* createGrass(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, Matrix *playerPos) {
        Grass* grass = new Grass(sm, core, "models/grass.gem");
        grass->init(core, worldPositions, playerPos);
        return grass;
    }
};