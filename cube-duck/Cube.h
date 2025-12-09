#pragma once

#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Camera.h"

#define NUM_SUBDIVISIONS 8
#define ROUNDNESS_FACTOR 0.25f

struct CubePixelShaderCB {
    Vec3 topColor;
    Vec3 bottomColor;
    float heightThreshold;
    Vec3 lightDirection;
    float lightStrength;
};

CubePixelShaderCB GrassCubePixelShader = {
    Vec3(0.2, 1.0, 0.2),           // topColor (green)
    Vec3(0.45, 0.2, 0.05),         // bottomColor (brown)
    0.9f,                          // heightThreshold
    Vec3(0.4, 1.0, 0.3),           // lightDirection
    0.7f                           // lightingStrength
};

class Cube {
public:
    Mesh mesh;
    ShaderManager* shaderManager;
    PSOManager psos;
    VertexLayoutCache vertexLayoutCache;
    CubePixelShaderCB* pixelShaderCB;
    VertexDefaultShaderCB* vertexShaderCB;

    Cube(ShaderManager* sm, CubePixelShaderCB *pixelShaderCB, VertexDefaultShaderCB *vertexShader = nullptr) : shaderManager(sm), pixelShaderCB(pixelShaderCB) {
        if (vertexShader == nullptr) {
            vertexShader = new VertexDefaultShaderCB();
            vertexShader->W.setIdentity();
            vertexShader->VP.setIdentity();
        }
        vertexShaderCB = vertexShader;
    }

    STATIC_VERTEX makeVertex(Vec3 pos) {
        STATIC_VERTEX v;
        v.pos = pos;
        v.tu = 0;
        v.tv = 0;
        v.tangent = Vec3(1,0,0); // not used
        v.normal = Vec3(0,0,0);
        return v;
    }

    void init(Core* core) {
        std::vector<STATIC_VERTEX> vertices;
        std::vector<unsigned int> indices;

        auto addFace = [&](Vec3 v00, Vec3 v10, Vec3 v01) {
            int startIndex = vertices.size();

            for (int y = 0; y <= NUM_SUBDIVISIONS; y++) {
                float fy = y / NUM_SUBDIVISIONS;
                Vec3 rowStart = v00 * (1 - fy) + v01 * fy;
                Vec3 rowEnd   = v10 * (1 - fy) + (v01 + (v10 - v00)) * fy;

                for (int x = 0; x <= NUM_SUBDIVISIONS; x++) {
                    float fx = x / NUM_SUBDIVISIONS;
                    Vec3 pos = rowStart * (1 - fx) + rowEnd * fx;
                    vertices.push_back(makeVertex(pos));
                }
            }

            // indices
            for (int y = 0; y < NUM_SUBDIVISIONS; y++) {
                for (int x = 0; x < NUM_SUBDIVISIONS; x++) {
                    int i0 = startIndex + y * (NUM_SUBDIVISIONS+1) + x;
                    int i1 = i0 + 1;
                    int i2 = i0 + (NUM_SUBDIVISIONS+1);
                    int i3 = i2 + 1;
                    indices.push_back(i0);
                    indices.push_back(i1);
                    indices.push_back(i3);

                    indices.push_back(i0);
                    indices.push_back(i3);
                    indices.push_back(i2);
                }
            }
        };

        Vec3 p0(-1,-1,-1), p1(1,-1,-1), p2(1,1,-1), p3(-1,1,-1);
        Vec3 p4(-1,-1,1),  p5(1,-1,1),  p6(1,1,1),  p7(-1,1,1);

        // build cube
        addFace(p0, p1, p3); // BACK
        addFace(p5, p4, p6); // FRONT
        addFace(p4, p0, p7); // LEFT
        addFace(p1, p5, p2); // RIGHT
        addFace(p3, p2, p7); // TOP
        addFace(p4, p5, p0); // BOTTOM

        // ---- STEP 2: compute smooth normals ----
        for (auto &v : vertices)
            v.normal = v.pos.normalize();

        // ---- STEP 3: push geometry outward for roundness ----
        for (auto &v : vertices)
            v.pos = v.pos + v.normal * ROUNDNESS_FACTOR;

        // Mesh init
        mesh.initFromVec(core, vertices, indices);

        Shader* vertexShaderBlob = shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB);
        Shader* pixelShaderBlob = shaderManager->getPixelShader("shaders/pixel/CubePixelShader.hlsl", pixelShaderCB);
        psos.createPSO(core, "Cube", vertexShaderBlob->shaderBlob, pixelShaderBlob->shaderBlob, vertexLayoutCache.getStaticLayout());
    }

    void updateVertexShader(Core* core) {
        shaderManager->updateConstantVS("shaders/vertex/VertexShader.hlsl", "W",  &vertexShaderCB->W);
        shaderManager->updateConstantVS("shaders/vertex/VertexShader.hlsl", "VP", &vertexShaderCB->VP);
        shaderManager->getVertexShader("shaders/vertex/VertexShader.hlsl", vertexShaderCB)->apply(core);
    }

    void updatePixelShader(Core* core) {
        shaderManager->updateConstantVS("shaders/pixel/CubePixelShader.hlsl", "TopColor",     &pixelShaderCB->topColor);
        shaderManager->updateConstantVS("shaders/pixel/CubePixelShader.hlsl", "BottomColor",  &pixelShaderCB->bottomColor);
        shaderManager->updateConstantVS("shaders/pixel/CubePixelShader.hlsl", "HeightThreshold", &pixelShaderCB->heightThreshold);
        shaderManager->updateConstantVS("shaders/pixel/CubePixelShader.hlsl", "LightDirection",  &pixelShaderCB->lightDirection);
        shaderManager->updateConstantVS("shaders/pixel/CubePixelShader.hlsl", "LightStrength", &pixelShaderCB->lightStrength);
        shaderManager->getPixelShader("shaders/pixel/CubePixelShader.hlsl", pixelShaderCB)->apply(core);
    }

    void scale(float s) {
        vertexShaderCB->W = vertexShaderCB->W.setScaling(Vec3(s, s, s));
    }

    void updateFromCamera(Core* core, Camera* camera) {
        Matrix viewMatrix;
        viewMatrix.setLookatMatrix(camera->from, camera->to, camera->up);

        Matrix projectionMatrix;
        projectionMatrix.setProjectionMatrix(ZFAR, ZNEAR, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

        vertexShaderCB->VP = projectionMatrix.mul(viewMatrix);
    }

    void draw(Core* core, Camera* camera) {
        core->beginRenderPass();
        psos.bind(core, "Cube");

        updateFromCamera(core, camera);
        updateVertexShader(core);
        updatePixelShader(core);

        mesh.draw(core);
    }
};
