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

#define CUBE_MODEL_FILE "models/cube_5.gem"
#define CUBE_TEXTURED_VERTEX_SHADER "shaders/vertex/VertexShaderLightTexture.hlsl"
#define CUBE_TEXTURED_PIXEL_SHADER "shaders/pixel/PixelShaderLightTexture.hlsl"

#define BRICK_TEXTURE "models/textures/metal_color.png"

class CubeTextured : public GEMObject {
public:
    BRDFLightCB* lightCB;
    Texture *texture;
    std::string textureName;
    std::string filename;
    std::string vertexShaderFile;

    CubeTextured(ShaderManager* sm, Core* core, std::string _filename = CUBE_MODEL_FILE) : GEMObject(sm, core, _filename) {
        filename = _filename;
    }

    void init(Core* core, std::vector<Matrix> _worldPositions, BRDFLightCB* light, std::string textureFilename, std::string _vertexShaderFile = CUBE_TEXTURED_VERTEX_SHADER) {
        vertexShaderFile = _vertexShaderFile;
        worldPositions = _worldPositions;

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

        Shader* vShader = shaderManager->getVertexShader(vertexShaderFile, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(CUBE_TEXTURED_PIXEL_SHADER, lightCB);
        psos.createPSO(core, filename, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(CUBE_TEXTURED_PIXEL_SHADER, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(CUBE_TEXTURED_PIXEL_SHADER, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(CUBE_TEXTURED_PIXEL_SHADER, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(CUBE_TEXTURED_PIXEL_SHADER, lightCB)->apply(core);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant(vertexShaderFile, "W", &vertexShaderCB->W);
        shaderManager->updateConstant(vertexShaderFile, "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader(vertexShaderFile, vertexShaderCB)->apply(core);
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

    static CubeTextured* createBrickCubes(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
        CubeTextured* cubes = new CubeTextured(sm, core);
        cubes->init(core, worldPositions, light, BRICK_TEXTURE);
        return cubes;
    }
};