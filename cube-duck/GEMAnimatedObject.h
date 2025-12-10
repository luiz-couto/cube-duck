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
    ShaderManager* shaderManager;
    PSOManager psos;
    std::string filename;
    VertexShaderCBAnimatedModel* vertexShaderCB;

    GEMAnimatedObject(ShaderManager* sm, const std::string& filename) : shaderManager(sm), filename(filename) {}

    void init(Core* core, VertexShaderCBAnimatedModel* vsCB = nullptr) {
        if (vsCB == nullptr) {
            vsCB = new VertexShaderCBAnimatedModel();
            vsCB->W.setIdentity();
            vsCB->VP.setIdentity();
        }

        vertexShaderCB = vsCB;

        // Build geometry
        animatedModel = new AnimatedModel(core);
        animatedModel->load(filename);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/AnimatedVertexShader.hlsl", vsCB);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShaderTexture.hlsl", PIXEL_SHADER);

        psos.createPSO(core, filename, vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, AnimatedVertexLayoutCache::getAnimatedLayout());
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void updateBones(AnimationInstance* animationInstance) {
        memcpy(vertexShaderCB->bones, animationInstance->matrices, sizeof(vertexShaderCB->bones));
    }

    void scale(float s) {
        vertexShaderCB->W.setScaling(s, s, s);
    }

    void translate(const Vec3& t) {
        vertexShaderCB->W.setTranslation(t.x, t.y, t.z);
    }

    void rotateY(float angle) {
        vertexShaderCB->W.setRotationY(angle);
    }

    void draw(Core* core, Camera* camera, AnimationInstance* animationInstance) {
        core->beginRenderPass();

        // 1. Bind PSO FIRST
        psos.bind(core, filename);

        updateFromCamera(core, camera);
        updateBones(animationInstance);

        // 2. Update constant buffer values
        shaderManager->updateConstant("shaders/vertex/AnimatedVertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/AnimatedVertexShader.hlsl", "VP", &vertexShaderCB->VP);
        shaderManager->updateConstant("shaders/vertex/AnimatedVertexShader.hlsl", "bones", vertexShaderCB->bones);

        // 3. Apply vertex shader (binds CBV)
        shaderManager->getVertexShader("shaders/vertex/AnimatedVertexShader.hlsl", vertexShaderCB)->apply(core);

        // 4. Draw
        animatedModel->draw(shaderManager);
    }

};