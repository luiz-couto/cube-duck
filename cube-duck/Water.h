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

#define WATER_PLANE_MODEL_FILE "models/water_plane.gem"
#define WATER_VERTEX_SHADER "shaders/vertex/VertexShaderWater.hlsl"
#define WATER_PIXEL_SHADER "shaders/pixel/PixelShaderWater.hlsl"

#define WATER_TEXTURE "models/textures/sea_water.png"

class Water : public GEMObject {
public:
    BRDFLightCB* lightCB;
    Texture *texture;
    std::string textureName;
    std::string filename;

    Water(ShaderManager* sm, Core* core, std::string _filename = WATER_PLANE_MODEL_FILE) : GEMObject(sm, core, _filename) {
        filename = _filename;
    }

    void init(Core* core, std::vector<Matrix> worldPositions, BRDFLightCB* light, std::string textureFilename) {
        VertexDefaultShaderCB *vertexShader = new VertexDefaultShaderCB();
        vertexShader->W.setIdentity();
        vertexShader->VP.setIdentity();
        vertexShaderCB = vertexShader;

        lightCB = light;

        // Build geometry
        staticMesh.load(filename, worldPositions);

        // Load texture
        textureName = textureFilename;
        texture = new Texture();
        texture->load(core, textureName);

        Shader* vShader = shaderManager->getVertexShader(WATER_VERTEX_SHADER, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(WATER_PIXEL_SHADER, lightCB);
        psos.createPSO(core, filename, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout(), true);
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(WATER_PIXEL_SHADER, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(WATER_PIXEL_SHADER, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(WATER_PIXEL_SHADER, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(WATER_PIXEL_SHADER, lightCB)->apply(core);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant(WATER_VERTEX_SHADER, "W", &vertexShaderCB->W);
        shaderManager->updateConstant(WATER_VERTEX_SHADER, "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader(WATER_VERTEX_SHADER, vertexShaderCB)->apply(core);
    }

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        updateConstantsVertexShader(core);
        updateConstantsPixelShader(core);
        shaderManager->updateTexturePS(core, textureName, texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Water* createWater(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
        Water* water = new Water(sm, core);
        water->init(core, worldPositions, light, WATER_TEXTURE);
        return water;
    }
};