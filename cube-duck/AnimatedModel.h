#pragma once
#include <vector>
#include "GEMLoader.h"
#include "AnimatedMesh.h"
#include "Animation.h"
#include "PSOManager.h"
#include "Texture.h"

class AnimatedModel {
public:
    std::vector<AnimatedMesh*> meshes;
    Core* core;
    Animation animation;
    std::vector<std::string> textureFilenames;
    std::vector<Texture*> textures;
    std::string name;

    AnimatedModel(Core* core) : core(core) {}

    void load(const std::string& filename) {
        name = filename;
        GEMLoader::GEMModelLoader loader;
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        GEMLoader::GEMAnimation gemanimation;
        loader.load(filename, gemmeshes, gemanimation);

        for (int i = 0; i < gemmeshes.size(); i++) {
            AnimatedMesh* mesh = new AnimatedMesh();
            std::vector<ANIMATED_VERTEX> vertices;
            for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
                ANIMATED_VERTEX v;
                memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
                vertices.push_back(v);
            }
            std::string texFilename = gemmeshes[i].material.find("albedo").getValue();
            textureFilenames.push_back(texFilename);

            // Load the texture. Use the texture manager to avoid loading duplicates
            Texture* texture = new Texture();
            texture->load(core, texFilename);
            textures.push_back(texture);

            mesh->init(core, vertices, gemmeshes[i].indices);
            meshes.push_back(mesh);
        }

        memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));

        // Load animation data - bones
        for (int i = 0; i < gemanimation.bones.size(); i++) {
            Bone bone;
            bone.name = gemanimation.bones[i].name;
            memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
            bone.parentIndex = gemanimation.bones[i].parentIndex;
            animation.skeleton.bones.push_back(bone);
        }

        std::string allAnimationNames;

        // Load animation data - animations
        for (int i = 0; i < gemanimation.animations.size(); i++) {
            std::string name = gemanimation.animations[i].name;
            AnimationSequence aseq;
            aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
            for (int n = 0; n < gemanimation.animations[i].frames.size(); n++) {
                AnimationFrame frame;
                for (int index = 0; index < gemanimation.animations[i].frames[n].positions.size(); index++) {
                    Vec3 p;
                    Quaternion q;
                    Vec3 s;
                    memcpy(&p, &gemanimation.animations[i].frames[n].positions[index], sizeof(Vec3));
                    frame.positions.push_back(p);
                    memcpy(&q, &gemanimation.animations[i].frames[n].rotations[index], sizeof(Quaternion));
                    frame.rotations.push_back(q);
                    memcpy(&s, &gemanimation.animations[i].frames[n].scales[index], sizeof(Vec3));
                    frame.scales.push_back(s);
                }
                aseq.frames.push_back(frame);
            }
            animation.animations.insert({ name, aseq });
            allAnimationNames += name + "\n";
        }
        //MessageBoxA(NULL, allAnimationNames.c_str(), "Loaded Animations", MB_OK);
    }

    void draw(ShaderManager* shaderManager) {
        for (int i = 0; i < meshes.size(); i++) {
            if (i < textures.size() && textures[i] != nullptr) {
                shaderManager->updateTexturePS(core, name, textures[i]->heapOffset);
            }
            meshes[i]->draw(core);
        }
    }
};