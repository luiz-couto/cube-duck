#pragma once

#include <print>
#include "Animation.h"
#include "AnimatedMesh.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Window.h"

#define BULL_MODEL_FILE "models/Bull-white.gem"
#define CAT_MODEL_FILE "models/Cat-Orange.gem"
#define CAT_DARK_MODEL_FILE "models/Cat-Dark.gem"
#define CAT_SIAMESE_MODEL_FILE "models/Cat-Siamese.gem"
#define DUCK_BROWN_MODEL_FILE "models/Duck-brown.gem"
#define DUCK_MIXED_MODEL_FILE "models/Duck-mixed.gem"
#define E_WALK_VELOCITY 0.09f
#define E_LOADING_FRAME 3
#define E_ENEMY_BOX_SIZE 2.0f

enum ENEMY_ANIMATION {
    E_IDLE_VARIATION,
    E_WALK_FORWARD,
    E_TURN_90_LEFT,
    E_TURN_90_RIGHT,
    E_RUN_FORWARD,
    E_WALK_BACKWARDS,
    E_ATTACK,
    E_HIT_REACTION,
    E_BIRD_IDLE_VARIATION
};

enum MOVE_KIND {
    ALONG_X,
    ALONG_Z,
};

const char *E_AnimationsMap[] = { "idle variation", "walk forward", "turn 90 l", "turn 90 r", "run forward", "walk backwards", "attack01", "hit reaction", "bird idle variation" };

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
    float walkVelocity;

    Vec3 size = {E_ENEMY_BOX_SIZE, E_ENEMY_BOX_SIZE, E_ENEMY_BOX_SIZE};

    Enemy(ShaderManager *_sm, Core *_core, Vec3 _startPosition, Vec3 _endPosition, MOVE_KIND _moveKind, float _rotationAngle, std::string enemyFile, float _scale, ENEMY_ANIMATION animation, float _walkVelocity = E_WALK_VELOCITY): 
        sm(_sm), core(_core), position(_startPosition), startPosition(_startPosition), 
        endPosition(_endPosition), moveKind(_moveKind), rotationAngle(_rotationAngle), 
        walkVelocity(_walkVelocity), enemyModel(sm, enemyFile)
    {
        
        enemyModel.init(core, &vsCBAnimatedModel);
        animatedInstance.init(&enemyModel.animatedModel->animation, 0);
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));

        scale.setScaling(_scale, _scale, _scale);
        currentAnimation = animation;
    }

    void resetPosition() {
        position = startPosition;
    }

    void setSize(Vec3 newSize) {
        size = newSize;
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
            bool reached = false;
            if (endPosition.x - position.x < 0) {
                position.x -= walkVelocity;
                if (position.x <= endPosition.x) {
                    reached = true;
                }
            }
            if (endPosition.x - position.x >= 0) {
                position.x += walkVelocity;
                if (position.x >= endPosition.x) {
                    reached = true;
                }
            }

            if (reached) { // reached goal
                Vec3 aux = startPosition;
                startPosition = endPosition;
                endPosition = aux;
                rotationAngle = (rotationAngle + 180) % 360;
            }
        }

        if (moveKind == ALONG_Z && std::abs(position.z - endPosition.z) > 0.001) {
            bool reached = false;
            if (endPosition.z - position.z < 0) {
                position.z -= walkVelocity;
                if (position.z <= endPosition.z) {
                    reached = true;
                }
            }
            if (endPosition.z - position.z >= 0) {
                position.z += walkVelocity;
                if (position.z >= endPosition.z) {
                    reached = true;
                }
            }

            if (reached) { // reached goal
                Vec3 aux = startPosition;
                startPosition = endPosition;
                endPosition = aux;
                rotationAngle = (rotationAngle + 180) % 360;
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