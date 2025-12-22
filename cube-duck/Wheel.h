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

#define WHEEL_MODEL_FILE "models/waterwheel.gem"
#define WHEEL_VERTEX_SHADER "shaders/vertex/VertexShaderWaterWheel.hlsl"
#define WHEEL_PIXEL_SHADER "shaders/pixel/PixelShaderLightTexture.hlsl"

#define WHEEL_TEXTURE "models/textures/ColorPalette.png"

struct WheelVertexShaderCB {
    Matrix W;
    Matrix VP;
    float timeDt;
};

class Wheel : public GEMObject {
public:
    BRDFLightCB* lightCB;
    WheelVertexShaderCB* vertexShaderCB;
    Texture *texture;
    std::string textureName;
    std::string filename;

    Wheel(ShaderManager* sm, Core* core, std::string _filename = WHEEL_MODEL_FILE) : GEMObject(sm, core, _filename) {
        filename = _filename;
    }

    void init(Core* core, std::vector<Matrix> worldPositions, BRDFLightCB* light, std::string textureFilename) {
        WheelVertexShaderCB *vertexShader = new WheelVertexShaderCB();
        vertexShader->W.setIdentity();
        vertexShader->VP.setIdentity();
        vertexShader->timeDt = 0.0f;
        vertexShaderCB = vertexShader;

        lightCB = light;

        // Build geometry
        staticMesh.load(filename, worldPositions);

        // Load texture
        textureName = textureFilename;
        texture = new Texture();
        texture->load(core, textureName);

        Shader* vShader = shaderManager->getVertexShader(WHEEL_VERTEX_SHADER, vertexShaderCB);
        Shader* pShader = shaderManager->getPixelShader(WHEEL_PIXEL_SHADER, lightCB);
        psos.createPSO(core, filename, vShader->shaderBlob, pShader->shaderBlob, vertexLayoutCache.getStaticLayout(), false);
    }

    void updateConstantsPixelShader(Core* core) {
        shaderManager->updateConstant(WHEEL_PIXEL_SHADER, "LightColor", &lightCB->lightColor);
        shaderManager->updateConstant(WHEEL_PIXEL_SHADER, "LightDirection", &lightCB->lightDirection);
        shaderManager->updateConstant(WHEEL_PIXEL_SHADER, "LightStrength", &lightCB->lightStrength);

        shaderManager->getPixelShader(WHEEL_PIXEL_SHADER, lightCB)->apply(core);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant(WHEEL_VERTEX_SHADER, "W", &vertexShaderCB->W);
        shaderManager->updateConstant(WHEEL_VERTEX_SHADER, "VP", &vertexShaderCB->VP);
        shaderManager->updateConstant(WHEEL_VERTEX_SHADER, "time", &vertexShaderCB->timeDt);

        shaderManager->getVertexShader(WHEEL_VERTEX_SHADER, vertexShaderCB)->apply(core);
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void draw(Core* core, Camera* camera, float dt) {
        vertexShaderCB->timeDt += dt;

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

    static Wheel* createWheel(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions, BRDFLightCB *light) {
        Wheel* wheel = new Wheel(sm, core);
        wheel->init(core, worldPositions, light, WHEEL_TEXTURE);
        return wheel;
    }
};