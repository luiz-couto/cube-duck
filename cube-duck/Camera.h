#pragma once

#include "Math.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define ZFAR 1000.0f
#define ZNEAR 0.01f
#define FOV 60.0f

#define CAMERA_MAX_Y 10.0f
#define CAMERA_MIN_Y -10.0f

#define CAMERA_MAX_X 10.0f
#define CAMERA_MIN_X -10.0f

class Camera {
    public:
    Vec3 from;
    Vec3 to;
    Vec3 up;

    Camera() : from(8.0f, 6.5f, 8.0f), to(0, 1, 0), up(0, 1, 0) {}
    Camera(Vec3 from, Vec3 to, Vec3 up) : from(from), to(to), up(up) {}

    void moveCameraY(float value) {
        float newY = to.y + value;
        if (newY < CAMERA_MIN_Y || newY > CAMERA_MAX_Y) return;
        to = Vec3(to.x, newY, to.z);
    }

    void moveCameraXZ(float value) {
        float newX = to.x + value;
        if (newX < CAMERA_MIN_X || newX > CAMERA_MAX_X) return;
        to = Vec3(newX, to.y, to.z);
    }

    void rotate(float angle) {
        // Calculate the vector from target to camera
        Vec3 offset = from - to;
        
        // Rotate around Y axis using rotation matrix
        float cosA = cosf(angle);
        float sinA = sinf(angle);
        float newX = offset.x * cosA - offset.z * sinA;
        float newZ = offset.x * sinA + offset.z * cosA;
        
        // Update camera position, keeping Y unchanged
        from = Vec3(newX + to.x, from.y, newZ + to.z);
    }
};