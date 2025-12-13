#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "GEMObject.h"
#include "Texture.h"

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
    Texture *texture;
    Grass(ShaderManager* sm, Core* core, const std::string& filename) : GEMObject(sm, core, filename) {}

    void init(Core* core, std::vector<Matrix> worldPositions, VertexDefaultShaderCB* vertexShader = nullptr, GrassPixelShaderCB* pixelShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
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
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        updateConstantsVertexShader(core);

        shaderManager->updateTexturePS(core, "Grass", texture->heapOffset);
        
        // 4. Draw
        staticMesh.draw();
    }

    static Grass* createGrass(ShaderManager* sm, Core* core, std::vector<Matrix> worldPositions) {
        Grass* grass = new Grass(sm, core, "models/grass.gem");
        grass->init(core, worldPositions);
        return grass;
    }
};