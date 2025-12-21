#pragma once

#include "Math.h"
#include <print>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define ZFAR 1000.0f
#define ZNEAR 0.01f
#define FOV 60.0f

#define CAMERA_MAX_Y 10.0f
#define CAMERA_MIN_Y -10.0f

#define CAMERA_MAX_X 10.0f
#define CAMERA_MIN_X -10.0f

#define MIN_ZOOM_Y 30.0f
#define MAX_ZOOM_Y 55.0f

#define MIN_ANGLE_Z 0.6
#define MAX_ANGLE_Z 1.3

class Camera {
    public:
    Vec3 startFrom;

    Vec3 from;
    Vec3 to;
    Vec3 up;

    float angleY = 0.0f;
    float angleZ = 0.0f;
    float distance = 0.0f;

    Camera() : from(16.3f, 23.7f, -17.0f), to(-2, 3, -1), up(0, 1, 0) {
        init();
    }
    Camera(Vec3 from, Vec3 to, Vec3 up) : from(from), to(to), up(up) {
        init();
    }

    void moveCameraY(float value) {
        float newY = to.y + value;
        if (newY < CAMERA_MIN_Y || newY > CAMERA_MAX_Y) return;
        to = Vec3(to.x, newY, to.z);
    }

    void init() {
        Vec3 offset = from - to;
        distance = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
        angleY = atan2(offset.z, offset.x);
        angleZ = acos(offset.y / distance);
    }

    void recalculateFrom() {
        Vec3 offset;
        offset.x = distance * sin(angleZ) * cos(angleY);
        offset.y = distance * cos(angleZ);
        offset.z = distance * sin(angleZ) * sin(angleY);

        from = to + offset;
    }

    void moveCameraX(float value) {
        Vec3 forward = (to - from).normalize();
        Vec3 right = (forward.cross(up)).normalize();

        Vec3 newTo = to + (right * value);

        if (newTo.x < CAMERA_MIN_X || newTo.x > CAMERA_MAX_X) return;
        if (newTo.z < CAMERA_MIN_X || newTo.z > CAMERA_MAX_X) return;

        float toDeltaX = to.x - newTo.x;
        float toDeltaZ = to.z - newTo.z;
        to = Vec3(to.x + toDeltaX, newTo.y, to.z + toDeltaZ);
    }

    void rotateY(float angle) {
        angleY += angle * 0.5;
        recalculateFrom();
    }

    void rotateZ(float angle) {
        float newAngle = angleZ - (angle * 0.5);
        if (newAngle < MIN_ANGLE_Z || newAngle > MAX_ANGLE_Z) {
            return;
        }
        angleZ = newAngle;
        recalculateFrom();
    }

    void zoom(float delta) {
        float newDistance = distance + delta;
        if (newDistance < MIN_ZOOM_Y || newDistance > MAX_ZOOM_Y) return;
        
        distance = newDistance;
        recalculateFrom();
    }

    void resetCamera() {
        from = Vec3(16.3f, 23.7f, -17.0f);
        to = Vec3(-2, 3, -1);
        init();
    }

    Vec3 getForwardVector() {
        Vec3 forward = (to - from).normalize();
        return forward;
    }

    Vec3 getRightVector() {
        Vec3 forward = getForwardVector();
        Vec3 right = (forward.cross(up)).normalize();
        return right;
    }
};