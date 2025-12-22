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

    Vec3 getGridLockedDirection() {
        Vec3 forward = camera->getForwardVector();

        // East or West
        if (std::abs(forward.x) > std::abs(forward.z)) {
            return forward.x > 0 ? Vec3(1, 0, 0) : Vec3(-1, 0, 0);
        }
        
        // North or South
        return forward.z > 0 ? Vec3(0, 0, 1) : Vec3(0, 0, -1);
    }

    void updateDuckRotation(Vec3 direction) {
        if (direction.x > 0) rotationAngle = 270;
        else if (direction.x < 0) rotationAngle = 90;
        else if (direction.z > 0) rotationAngle = 180;
        else if (direction.z < 0) rotationAngle = 0;
    }

    void reactToMovementKeys(Window *win) {
        if (isDead) return;
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
            Vec3 direction = getGridLockedDirection();
            position += direction * RUN_VELOCITY;
            updateDuckRotation(direction);
            currentAnimation = RUN_FORWARD;
        }

        if (win->keys['S']) {
            Vec3 direction = getGridLockedDirection();
            position -= direction * RUN_VELOCITY;
            Vec3 oppositeDir = Vec3(-direction.x, 0, -direction.z);
            updateDuckRotation(oppositeDir);
            currentAnimation = RUN_FORWARD;
        }

        if (win->keys['D']) {
            Vec3 direction = getGridLockedDirection();
            Vec3 leftDirection = Vec3(-direction.z, 0, direction.x);
            position += leftDirection * RUN_VELOCITY;
            updateDuckRotation(leftDirection);
            currentAnimation = RUN_FORWARD;
        }

        if (win->keys['A']) {
            Vec3 direction = getGridLockedDirection();
            Vec3 rightDirection = Vec3(direction.z, 0, -direction.x);
            position += rightDirection * RUN_VELOCITY;
            updateDuckRotation(rightDirection);
            currentAnimation = RUN_FORWARD;
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
        float duckHalfY = 1.0f;
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