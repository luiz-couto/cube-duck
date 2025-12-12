#pragma once

#include "Animation.h"
#include "AnimatedMesh.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"

#define DUCK_MODEL_FILE "models/Duck-white.gem"

enum DUCK_ANIMATION {
    IDLE_VARIATION,
    WALK
};

const char *AnimationsMap[] = { "idle variation", "walk" };

class Duck {
public:
    ShaderManager *sm;
    Core *core;

    Vec3 position;
    AnimationInstance animatedInstance;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;
    GEMAnimatedObject duckModel;

    DUCK_ANIMATION currentAnimation;

    Duck(ShaderManager *_sm, Core *_core, Vec3 _position = {1.0f, 8.0f, 1.0f}): sm(_sm), core(_core), position(_position), duckModel(sm, DUCK_MODEL_FILE) {
        duckModel.init(core, &vsCBAnimatedModel);
        animatedInstance.init(&duckModel.animatedModel->animation, 0);
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));

        currentAnimation = IDLE_VARIATION;
    }

    void updateAnimation(float dt) {
        animatedInstance.update(AnimationsMap[currentAnimation], dt);
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}
    }

    void draw(Camera *camera) {
        duckModel.scale(0.02f);
        duckModel.translate(position);
        duckModel.draw(core, camera, &animatedInstance);
    }
};