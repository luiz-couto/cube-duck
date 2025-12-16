#pragma once

#include "CubeTextured.h"

#define SPEED_FACTOR 100.0f

class Coin : public CubeTextured {
public:
    void draw(Core* core, Camera* camera, float timeElapsed) {
        // Rotate coin around Y axis based on time
        Matrix rotation;
        rotation.setRotationY(timeElapsed * SPEED_FACTOR);
        vertexShaderCB->W = rotation;

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
};