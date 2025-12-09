#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "AnimatedModel.h"
#include "Animation.h"

class GEMAnimatedObject {
public:
    AnimatedModel* animatedModel;
    AnimationInstance* animationInstance;
    ShaderManager* shaderManager;
    PSOManager psos;
    std::string filename;

    GEMAnimatedObject(ShaderManager* sm, const std::string& filename) : shaderManager(sm), filename(filename) {}

    void init(Core* core, VertexShaderCBAnimatedModel* vsCB) {
        // Build geometry
        animatedModel = new AnimatedModel(core);
        animatedModel->load(filename);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/AnimatedVertexShader.hlsl", vsCB);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShaderTexture.hlsl", PIXEL_SHADER);

        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, AnimatedVertexLayoutCache::getAnimatedLayout());
    }

    void draw(Core* core, VertexShaderCBAnimatedModel* vsCB) {
        core->beginRenderPass();

        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        // 2. Update constant buffer values
        shaderManager->updateConstantVS("shaders/vertex/AnimatedVertexShader.hlsl", "W", &vsCB->W);
        shaderManager->updateConstantVS("shaders/vertex/AnimatedVertexShader.hlsl", "VP", &vsCB->VP);
        shaderManager->updateConstantVS("shaders/vertex/AnimatedVertexShader.hlsl", "bones", vsCB->bones);

        // 3. Apply vertex shader (binds CBV)
        shaderManager->getVertexShader("shaders/vertex/AnimatedVertexShader.hlsl", vsCB)->apply(core);

        // 4. Draw
        animatedModel->draw(shaderManager);
    }

};