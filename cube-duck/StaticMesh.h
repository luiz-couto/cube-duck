#pragma once
#include <vector>
#include "Mesh.h"
#include "GEMLoader.h"

class StaticMesh {
public:
    std::vector<Mesh*> meshes;
    Core* core;

    StaticMesh(Core* core) : core(core) {}

    void load(const std::string& filename) {
        GEMLoader::GEMModelLoader loader;
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        loader.load(filename, gemmeshes);
        for (int i = 0; i < gemmeshes.size(); i++) {
            Mesh* mesh = new Mesh();
            std::vector<STATIC_VERTEX> vertices;
            for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
                STATIC_VERTEX v;
                memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
                vertices.push_back(v);
            }
            mesh->initFromVec(core, vertices, gemmeshes[i].indices);
            meshes.push_back(mesh);
        }
    }

    void draw() {
        for (int i = 0; i < meshes.size(); i++) {
            meshes[i]->draw(core);
        }
    }
};