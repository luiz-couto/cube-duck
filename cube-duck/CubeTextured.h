#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "GEMObject.h"
#include "Texture.h"

#define CUBE_MODEL_FILE "models/cube_2.gem"
#define VERTEX_SHADER "shaders/vertex/VertexShaderLightTexture.hlsl"
#define PIXEL_SHADER "shaders/pixel/PixelShaderLightTexture.hlsl"

#define BRICK_TEXTURE "models/textures/T_Animalstextures_nh.png"

struct BRDFLightCB {
    Vec3 lightColor;
    Vec3 lightDirection;
    float lightStrength;
};

class CubeTextured : public GEMObject {
public:
    BRDFLightCB* lightCB;
    Texture *texture;
    std::string textureName;

    CubeTextured(ShaderManager* sm, Core* core) : GEMObject(sm, core, CUBE_MODEL_FILE) {}

    void init(Core* core, std::vector<Matrix> worldPositions, BRDFLightCB* light, std::string textureFilename) {
        VertexDefaultShaderCB *vertexShader = new VertexDefaultShaderCB();
        vertexShader->W.setIdentity();
        vertexShader->VP.setIdentity();
        vertexShaderCB = vertexShader;

        lightCB = light;

        // Build geometry
        staticMesh.load(CUBE_MODEL_FILE, worldPositions);

        // Load texture
        textureName = textureFilename;
        texture = new Texture();
        texture->load(core, textureName);

        Shader* vShader = shaderManager->getVertexShader(VERTEX_SHADER, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(PIXEL_SHADER, lightCB);
        psos.createPSO(core, CUBE_MODEL_FILE, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(PIXEL_SHADER, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(PIXEL_SHADER, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(PIXEL_SHADER, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(PIXEL_SHADER, lightCB)->apply(core);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant(VERTEX_SHADER, "W", &vertexShaderCB->W);
        shaderManager->updateConstant(VERTEX_SHADER, "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader(VERTEX_SHADER, vertexShaderCB)->apply(core);
    }

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, CUBE_MODEL_FILE);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        updateConstantsVertexShader(core);
        updateConstantsPixelShader(core);
        shaderManager->updateTexturePS(core, textureName, texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static CubeTextured* createBrickCubes(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
        CubeTextured* cubes = new CubeTextured(sm, core);
        cubes->init(core, worldPositions, light, BRICK_TEXTURE);
        return cubes;
    }
};