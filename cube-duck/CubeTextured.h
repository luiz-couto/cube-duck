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

#define CUBE_MODEL_FILE "models/cube_7.gem"
#define CUBE_TEXTURED_VERTEX_SHADER "shaders/vertex/VertexShaderLightTexture.hlsl"
#define CUBE_TEXTURED_PIXEL_SHADER "shaders/pixel/PixelShaderLightTexture.hlsl"
#define CUBE_TEXTURED_PIXEL_SHADER_NORMAL "shaders/pixel/PixelShaderLightTextureNormalMap.hlsl"

#define BRICK_TEXTURE "models/textures/metal_color.png"
#define BRICK_NORMAL_MAP "models/textures/normal3.png"

class CubeTextured : public GEMObject {
public:
    BRDFLightCB* lightCB;
    Texture *texture;
    Texture *normalMap;
    std::string textureName;
    std::string normalMapName = "";
    std::string filename;
    std::string vertexShaderFile;
    std::string pixelShaderFile;

    CubeTextured(ShaderManager* sm, Core* core, std::string _filename = CUBE_MODEL_FILE) : GEMObject(sm, core, _filename) {
        filename = _filename;
    }

    void init(Core* core, std::vector<Matrix> _worldPositions, BRDFLightCB* light, std::string textureFilename, std::string normalMapFilename = "", std::string _vertexShaderFile = CUBE_TEXTURED_VERTEX_SHADER) {
        if (normalMapFilename != "") {
            pixelShaderFile = CUBE_TEXTURED_PIXEL_SHADER_NORMAL;
        } else {
            pixelShaderFile = CUBE_TEXTURED_PIXEL_SHADER;
        }
        
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

        if (normalMapFilename != "") {
            normalMapName = normalMapFilename;
            normalMap = new Texture();
            normalMap->load(core, normalMapFilename);
        }

        Shader* vShader = shaderManager->getVertexShader(vertexShaderFile, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(pixelShaderFile, lightCB);
        psos.createPSO(core, filename, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(pixelShaderFile, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(pixelShaderFile, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(pixelShaderFile, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(pixelShaderFile, lightCB)->apply(core);
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

        if (normalMapName != "") {
            shaderManager->updateTexturesPSWithNormalMap(core, textureName, texture->heapOffset, normalMapName, normalMap->heapOffset);
        } else {
            shaderManager->updateTexturePS(core, textureName, texture->heapOffset);
        }
        
        // 4. Draw
        staticMesh.draw();
    }

    static CubeTextured* createBrickCubes(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
        CubeTextured* cubes = new CubeTextured(sm, core);
        cubes->init(core, worldPositions, light, BRICK_TEXTURE);
        return cubes;
    }
};