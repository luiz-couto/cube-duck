#pragma once

#include <print>
#include "Animation.h"
#include "AnimatedMesh.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Window.h"

#define ENEMY_MODEL_FILE "models/Bull-white.gem"
#define E_WALK_VELOCITY 0.08f
#define E_LOADING_FRAME 11
#define E_ENEMY_BOX_SIZE 1.5

enum ENEMY_ANIMATION {
    E_IDLE_VARIATION,
    E_WALK_FORWARD,
    E_TURN_90_LEFT,
    E_TURN_90_RIGHT,
    E_RUN_FORWARD,
    E_WALK_BACKWARDS,
    E_ATTACK,
    E_HIT_REACTION
};

enum MOVE_KIND {
    ALONG_X,
    ALONG_Z,
};

const char *E_AnimationsMap[] = { "idle variation", "walk forward", "turn 90 l", "turn 90 r", "run forward", "walk backwards", "attack01", "hit reaction" };

class Enemy {
public:
    ShaderManager *sm;
    Core *core;

    Vec3 position;
    int rotationAngle = 0;
    int loadingFrame = 0;
    AnimationInstance animatedInstance;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;
    GEMAnimatedObject enemyModel;

    Matrix rotation;
    Matrix scale;
    Matrix translation;

    ENEMY_ANIMATION currentAnimation;

    Vec3 startPosition;
    Vec3 endPosition;
    MOVE_KIND moveKind;

    Enemy(ShaderManager *_sm, Core *_core, Vec3 _startPosition, Vec3 _endPosition, MOVE_KIND _moveKind, float _rotationAngle): 
        sm(_sm), core(_core), position(_startPosition), startPosition(_startPosition), 
        endPosition(_endPosition), moveKind(_moveKind), rotationAngle(_rotationAngle), enemyModel(sm, ENEMY_MODEL_FILE) 
    {
        
        enemyModel.init(core, &vsCBAnimatedModel);
        animatedInstance.init(&enemyModel.animatedModel->animation, 0);
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));

        scale.setScaling(0.015f, 0.015f, 0.015f);
        currentAnimation = E_WALK_FORWARD;
    }

    void resetPosition() {
        position = startPosition;
    }

    void move() {
        if (loadingFrame < E_LOADING_FRAME) {
            loadingFrame++;
            return;
        }
        if (loadingFrame == E_LOADING_FRAME) {
            loadingFrame = 0;
        }

        if (moveKind == ALONG_X && std::abs(position.x - endPosition.x) > 0.001) {
            if (endPosition.x - startPosition.x < 0) position.x -= E_WALK_VELOCITY;
            if (endPosition.x - startPosition.x >= 0) position.x += E_WALK_VELOCITY;

            if (std::abs(position.x - endPosition.x) < 0.001) { // reached goal
                Vec3 aux = startPosition;
                startPosition = endPosition;
                endPosition = aux;
                rotationAngle = -rotationAngle;
            }
        }

        if (moveKind == ALONG_Z && std::abs(position.z - endPosition.z) > 0.001) {
            if (endPosition.z - startPosition.z < 0) position.z -= E_WALK_VELOCITY;
            if (endPosition.z - startPosition.z >= 0) position.z += E_WALK_VELOCITY;

            if (std::abs(position.z - endPosition.z) < 0.001) { // reached goal
                Vec3 aux = startPosition;
                startPosition = endPosition;
                endPosition = aux;
                rotationAngle = -rotationAngle;
            }
        }        
    }

    void updateAnimation(Window *win, float dt) {
        move();
        animatedInstance.update(E_AnimationsMap[currentAnimation], dt);
        if (animatedInstance.animationFinished()) {
            animatedInstance.resetAnimationTime();
        }
    }

    void draw(Camera *camera) {
        rotation.setRotationY(rotationAngle);
        translation = translation.setTranslation(position);
        enemyModel.vertexShaderCB->W = (translation.mul(rotation)).mul(scale);
        enemyModel.draw(core, camera, &animatedInstance);
    }
};