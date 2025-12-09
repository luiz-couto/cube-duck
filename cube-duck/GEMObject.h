#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"

class GEMObject {
public:
    StaticMesh staticMesh;
    ShaderManager* shaderManager;
    PSOManager psos;
    VertexLayoutCache vertexLayoutCache;
    std::string filename;

    GEMObject(ShaderManager* sm, Core* core, const std::string& filename) : shaderManager(sm), staticMesh(core), filename(filename) {}

    void init(Core* core, VertexShaderCBStaticModel* vsCB) {
        // Build geometry
        staticMesh.load(filename);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vsCB);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShader.hlsl", PIXEL_SHADER);
        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void draw(Core* core, VertexShaderCBStaticModel* vsCB) {
        core->beginRenderPass();

        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        // 2. Update constant buffer values
        shaderManager->updateConstantVS("shaders/vertex/VertexShader.hlsl", "W", &vsCB->W);
        shaderManager->updateConstantVS("shaders/vertex/VertexShader.hlsl", "VP", &vsCB->VP);

        // 3. Apply vertex shader (binds CBV)
        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vsCB)->apply(core);
        // 4. Draw
        staticMesh.draw();
    }

};