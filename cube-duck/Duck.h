#pragma once

#include <print>
#include "Animation.h"
#include "AnimatedMesh.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Window.h"

#define DUCK_MODEL_FILE "models/Duck-white.gem"
#define RUN_VELOCITY 0.05f
#define WALK_VELOCITY 0.02f
#define LOADING_FRAME 15

enum DUCK_ANIMATION {
    IDLE_VARIATION,
    WALK_FORWARD,
    TURN_90_LEFT,
    TURN_90_RIGHT,
    RUN_FORWARD,
    WALK_BACKWARDS
};

const char *AnimationsMap[] = { "idle variation", "walk forward", "turn 90 l", "turn 90 r", "run forward", "walk backwards" };

class Duck {
public:
    ShaderManager *sm;
    Core *core;

    Vec3 position;
    int rotationAngle = 0;
    int loadingFrame = 0;
    AnimationInstance animatedInstance;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;
    GEMAnimatedObject duckModel;

    Matrix rotation;
    Matrix scale;
    Matrix translation;

    DUCK_ANIMATION currentAnimation;

    Duck(ShaderManager *_sm, Core *_core, Vec3 _position = {1.0f, 8.0f, 1.0f}): sm(_sm), core(_core), position(_position), duckModel(sm, DUCK_MODEL_FILE) {
        duckModel.init(core, &vsCBAnimatedModel);
        animatedInstance.init(&duckModel.animatedModel->animation, 0);
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));

        scale.setScaling(0.02f, 0.02f, 0.02f);
        currentAnimation = IDLE_VARIATION;
    }

    void reactToMovementKeys(Window *win) {
        if (win->keys['W']) {
            if (rotationAngle == 0 || rotationAngle == 360) position.z -= RUN_VELOCITY;
            if (rotationAngle == 90) position.x -= RUN_VELOCITY;
            if (rotationAngle == 180) position.z += RUN_VELOCITY;
            if (rotationAngle == 270) position.x += RUN_VELOCITY;

            currentAnimation = RUN_FORWARD;
        }

        if (win->keys['S']) {
            if (rotationAngle == 0 || rotationAngle == 360) position.z += WALK_VELOCITY;
            if (rotationAngle == 90) position.x += WALK_VELOCITY;
            if (rotationAngle == 180) position.z -= WALK_VELOCITY;
            if (rotationAngle == 270) position.x -= WALK_VELOCITY;

            currentAnimation = WALK_BACKWARDS;
        }


        if (loadingFrame < LOADING_FRAME) {
            loadingFrame++;
            return;
        }
        if (loadingFrame == LOADING_FRAME) {
            loadingFrame = 0;
        }

        if (win->keys['A']) {
            rotationAngle += 90;
            if (rotationAngle > 360) rotationAngle = 0;
            currentAnimation = TURN_90_LEFT;
        }

        if (win->keys['D']) {
            rotationAngle -= 90;
            if (rotationAngle < 0) rotationAngle = 270;
            currentAnimation = TURN_90_RIGHT;
        }
    }

    void updateAnimation(Window *win, float dt) {
        reactToMovementKeys(win);
        animatedInstance.update(AnimationsMap[currentAnimation], dt);
        if (animatedInstance.animationFinished()) {
            currentAnimation = IDLE_VARIATION;
            animatedInstance.resetAnimationTime();
        }
    }

    void draw(Camera *camera) {
        rotation.setRotationY(rotationAngle);
        translation = translation.setTranslation(position);
        duckModel.vertexShaderCB->W = (translation.mul(rotation)).mul(scale);
        duckModel.draw(core, camera, &animatedInstance);
    }
};