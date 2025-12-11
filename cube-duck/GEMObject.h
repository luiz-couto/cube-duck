#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"

class GEMObject {
public:
    StaticMesh staticMesh;
    ShaderManager* shaderManager;
    PSOManager psos;
    VertexLayoutCache vertexLayoutCache;
    std::string filename;
    VertexDefaultShaderCB* vertexShaderCB;

    GEMObject(ShaderManager* sm, Core* core, const std::string& filename) : shaderManager(sm), staticMesh(core), filename(filename) {}

    void init(Core* core, std::vector<Matrix> worldPositions, VertexDefaultShaderCB* vertexShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
        }
        vertexShaderCB = vertexShader;

        // Build geometry
        staticMesh.load(filename, worldPositions);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShaderNormals.hlsl", PIXEL_SHADER);
        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void scale(float s) {
        vertexShaderCB->W = vertexShaderCB->W.setScaling(Vec3(s, s, s));
    }

    void translate(const Vec3& t) {
        vertexShaderCB->W = vertexShaderCB->W.setTranslation(t);
    }

    void rotateY(float angle) {
        vertexShaderCB->W.setRotationY(angle);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void updateConstantsPixelShader(Core* core) {}

    void draw(Core* core, Camera* camera) {
        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);

        // 2. Update constant buffer values
        updateConstantsVertexShader(core);
        
        // 4. Draw
        staticMesh.draw();
    }
};