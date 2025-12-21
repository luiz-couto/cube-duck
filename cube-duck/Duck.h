#pragma once

#include <print>
#include "Animation.h"
#include "AnimatedMesh.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Window.h"

#define DUCK_MODEL_FILE "models/Duck-white.gem"
#define RUN_VELOCITY 0.04f
#define WALK_VELOCITY 0.02f
#define LOADING_FRAME 11
#define DUCK_BOX_SIZE 1.5
#define JUMP_HEIGHT 4.5f
#define JUMP_INCREMENT 0.13f
#define GRAVITY_PULL 0.05f

enum DUCK_ANIMATION {
    IDLE_VARIATION,
    WALK_FORWARD,
    TURN_90_LEFT,
    TURN_90_RIGHT,
    RUN_FORWARD,
    WALK_BACKWARDS,
    ATTACK,
    HIT_REACTION,
    DEATH
};

const char *AnimationsMap[] = { "idle variation", "walk forward", "turn 90 l", "turn 90 r", "run forward", "walk backwards", "attack01", "hit reaction", "death" };

class Duck {
public:
    ShaderManager *sm;
    Core *core;
    Camera *camera;

    Vec3 position;
    Vec3 lastPosition;
    int rotationAngle = 0;
    int loadingFrame = 0;
    AnimationInstance animatedInstance;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;
    GEMAnimatedObject duckModel;

    Matrix rotation;
    Matrix scale;
    Matrix translation;

    DUCK_ANIMATION currentAnimation;
    bool isJumping = false;
    bool isDead = false;
    float jumpingCurrentHeight = 0;

    Vec3 startPosition;

    Duck(ShaderManager *_sm, Core *_core, Vec3 _position, Camera *_camera): sm(_sm), core(_core), position(_position), camera(_camera), startPosition(_position), duckModel(sm, DUCK_MODEL_FILE) {
        duckModel.init(core, &vsCBAnimatedModel);
        animatedInstance.init(&duckModel.animatedModel->animation, 0);
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));

        scale.setScaling(0.02f, 0.02f, 0.02f);
        currentAnimation = IDLE_VARIATION;
    }

    void resetPosition() {
        position = startPosition;
    }

    void reactToMovementKeys(Window *win) {
        position.y -= GRAVITY_PULL;

        if (isJumping) {
            if (jumpingCurrentHeight < JUMP_HEIGHT) {
                position.y += JUMP_INCREMENT;
                jumpingCurrentHeight += JUMP_INCREMENT;
            }
        }

        if (!isJumping && jumpingCurrentHeight == 0 && win->keys[' ']) {
            position.y += JUMP_INCREMENT;
            isJumping = true;
            currentAnimation = HIT_REACTION;
        }

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
        if (isDead) {
            blockAllMovements();
            currentAnimation = DEATH;
        } else {
            reactToMovementKeys(win);
        }

        animatedInstance.update(AnimationsMap[currentAnimation], dt);
        if (animatedInstance.animationFinished()) {
            if (isDead) {
                isDead = false;
                resetPosition();
                camera->resetCamera();
            }
            currentAnimation = IDLE_VARIATION;
            animatedInstance.resetAnimationTime();
        }
    }

    void draw() {
        lastPosition = position;

        rotation.setRotationY(rotationAngle);
        translation = translation.setTranslation(position);
        duckModel.vertexShaderCB->W = (translation.mul(rotation)).mul(scale);
        duckModel.draw(core, camera, &animatedInstance);
    }

    void blockMovementX() {
        position.x = lastPosition.x;
    }

    void blockMovementY() {
        position.y = lastPosition.y;
    }

    void blockMovementZ() {
        position.z = lastPosition.z;
    }

    void blockAllMovements() {
        blockMovementX();
        blockMovementY();
        blockMovementZ();
    }

    bool checkBoxCollision(Vec3 axisPosition, Vec3 point, Vec3 size) {
        float duckHalfX = 0.5f;
        float duckHalfY = 0.8f;
        float duckHalfZ = 0.5f;
        
        bool overlapX = std::abs(axisPosition.x - point.x) < (duckHalfX + size.x / 2);
        bool overlapY = std::abs(axisPosition.y - point.y) < (duckHalfY + size.y / 2);
        bool overlapZ = std::abs(axisPosition.z - point.z) < (duckHalfZ + size.z / 2);
        return overlapX && overlapY && overlapZ;
    }

    bool checkCollisionX(Matrix *worldMatrix, Vec3 size) {
        Vec3 axisPosition = Vec3(position.x, lastPosition.y, lastPosition.z);
        Vec3 point = Vec3(worldMatrix->m[3], worldMatrix->m[7],worldMatrix->m[11]);
        return checkBoxCollision(axisPosition, point, size);
    }

    bool checkCollisionY(Matrix *worldMatrix, Vec3 size) {
        Vec3 axisPosition = Vec3(lastPosition.x, position.y, lastPosition.z);
        Vec3 point = Vec3(worldMatrix->m[3], worldMatrix->m[7],worldMatrix->m[11]);
        return checkBoxCollision(axisPosition, point, size);
    }

    bool checkCollisionZ(Matrix *worldMatrix, Vec3 size) {
        Vec3 axisPosition = Vec3(lastPosition.x, lastPosition.y, position.z);
        Vec3 point = Vec3(worldMatrix->m[3], worldMatrix->m[7], worldMatrix->m[11]);
        return checkBoxCollision(axisPosition, point, size);
    }
};