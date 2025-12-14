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

#define MIN_ZOOM_Y 5.0f
#define MAX_ZOOM_Y 40.0f

class Camera {
    public:
    Vec3 from;
    Vec3 to;
    Vec3 up;

    Camera() : from(8.0f, 10.0f, 8.0f), to(0, 1, 0), up(0, 1, 0) {}
    Camera(Vec3 from, Vec3 to, Vec3 up) : from(from), to(to), up(up) {}

    void moveCameraY(float value) {
        float newY = to.y + value;
        if (newY < CAMERA_MIN_Y || newY > CAMERA_MAX_Y) return;
        to = Vec3(to.x, newY, to.z);
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

    void rotate(float angle) {
        Vec3 offset = from - to;
        
        float cosA = cosf(angle);
        float sinA = sinf(angle);
        float newX = offset.x * cosA - offset.z * sinA;
        float newZ = offset.x * sinA + offset.z * cosA;

        from = Vec3(newX + to.x, from.y, newZ + to.z);
    }

    void zoom(float delta) {
        Vec3 direction = to - from;
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

        if (distance > 0.1f) {
            direction.x /= distance;
            direction.y /= distance;
            direction.z /= distance;

            float newDistance = distance + delta;
            Vec3 newFrom = Vec3(from.x + direction.x * delta, from.y + direction.y * delta, from.z + direction.z * delta);
            
            if (newFrom.y < MIN_ZOOM_Y || newFrom.y > MAX_ZOOM_Y ) return;
            from = newFrom;
        }
    }

    void resetCamera() {
        from = Vec3(8.0f, 6.5f, 8.0f);
        to = Vec3(0, 1, 0);
    }
};