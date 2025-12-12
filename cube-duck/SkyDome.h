#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Camera.h"

class SkyDome {
public:
    Mesh mesh;
    ShaderManager* shaderManager;
    PSOManager psos;
    VertexLayoutCache vertexLayoutCache;
    VertexDefaultShaderCB* vertexShaderCB;

    SkyDome(ShaderManager* sm) : shaderManager(sm) {}

    void init(Core* core, VertexDefaultShaderCB* vertexShader = nullptr) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
        }
        vertexShaderCB = vertexShader;

        // Build geometry
        std::vector<STATIC_VERTEX> vertices;
        float radius = 500.0f;
        int rings = 64;
        int segments = 64;
        float pi = 3.14159f;

        for (int lat = 0; lat <= rings; lat++) {
            float theta = lat * pi / rings;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            for (int lon = 0; lon <= segments; lon++) {
                float phi = lon * 2.0f * pi / segments;
                float sinPhi = sinf(phi);
                float cosPhi = cosf(phi);
                Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta,
                radius * sinTheta * sinPhi);
                Vec3 normal = position.normalize();
                float tu = 1.0f - (float)lon / segments;
                float tv = 1.0f - (float)lat / rings;
                vertices.push_back(addVertex(position, normal, tu, tv));
            }
        }

        std::vector<unsigned int> indices;
        for (int lat = 0; lat < rings; lat++) {
            for (int lon = 0; lon < segments; lon++) {
                int current = lat * (segments + 1) + lon;
                int next = current + segments + 1;
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);
                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        std::vector<Matrix> worldMatrices;
        Matrix identity;
        worldMatrices.push_back(identity);
        mesh.initFromVec(core, vertices, indices, worldMatrices);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShader);
        Shader* pixelShaderBlob = shaderManager->getShader("shaders/pixel/PixelShaderNormals.hlsl", PIXEL_SHADER);
        psos.createPSO(core, "SkyDome", vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void updateConstantsVertexShader(Core* core) {
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "W", &vertexShaderCB->W);
        shaderManager->updateConstant("shaders/vertex/VertexShader.hlsl", "VP", &vertexShaderCB->VP);

        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void draw(Core* core, Camera *camera) {
        core->beginRenderPass();

        psos.bind(core, "SkyDome");

        updateFromCamera(core, camera);
        updateConstantsVertexShader(core);

        mesh.draw(core);
    }

};