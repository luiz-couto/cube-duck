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

#define GRASS_MODEL "models/grass.gem"
#define GRASS_VERTEX_SHADER "shaders/vertex/GrassLightVertexShader.hlsl"
#define GRASS_PIXEL_SHADER "shaders/pixel/PixelShaderLightTexture.hlsl"
#define GRASS_TEXTURE "models/textures/Grass.png"

struct GrassVertexShaderCB {
    Matrix W;
    Matrix VP;
    Matrix PLAYER_POS;
};

class Grass : public GEMObject {
public:
    BRDFLightCB* lightCB;
    GrassVertexShaderCB* vertexShaderCB;
    Texture *texture;
    Grass(ShaderManager* sm, Core* core) : GEMObject(sm, core, GRASS_MODEL) {}

    void init(Core* core, std::vector<Matrix> worldPositions, Matrix *playerPos, BRDFLightCB* light) {
        GrassVertexShaderCB* vertexShader = new GrassVertexShaderCB();
        vertexShader->W.setIdentity();
        vertexShader->VP.setIdentity();
        vertexShader->PLAYER_POS = *playerPos;
        
        vertexShaderCB = vertexShader;

        lightCB = light;

        // load texture
        texture = new Texture();
        texture->load(core, GRASS_TEXTURE);

        // Build geometry
        staticMesh.load(GRASS_MODEL, worldPositions);

        Shader* vShader = shaderManager->getVertexShader(GRASS_VERTEX_SHADER, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(GRASS_PIXEL_SHADER, lightCB);

        psos.createPSO(core, GRASS_MODEL, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant(GRASS_VERTEX_SHADER, "W", &vertexShaderCB->W);
        shaderManager->updateConstant(GRASS_VERTEX_SHADER, "VP", &vertexShaderCB->VP);
        shaderManager->updateConstant(GRASS_VERTEX_SHADER, "PLAYER_POS", &vertexShaderCB->PLAYER_POS);

        shaderManager->getVertexShader(GRASS_VERTEX_SHADER, vertexShaderCB)->apply(core);
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(GRASS_PIXEL_SHADER, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(GRASS_PIXEL_SHADER, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(GRASS_PIXEL_SHADER, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(GRASS_PIXEL_SHADER, lightCB)->apply(core);
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
        psos.bind(core, GRASS_MODEL);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        vertexShaderCB->PLAYER_POS = *playerPos;
        updateConstantsVertexShader(core);
        updateConstantsPixelShader(core);

        shaderManager->updateTexturePS(core, GRASS_MODEL, texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Grass* createGrass(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, Matrix *playerPos, BRDFLightCB *light) {
        Grass* grass = new Grass(sm, core);
        grass->init(core, worldPositions, playerPos, light);
        return grass;
    }
};