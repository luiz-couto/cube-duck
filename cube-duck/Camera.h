#pragma once

#include "Math.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define ZFAR 1000.0f
#define ZNEAR 0.01f
#define FOV 60.0f

class Camera {
    public:
    Vec3 from;
    Vec3 to;
    Vec3 up;

    Camera() : from(11.0f, 5.0f, 11.0f), to(0, 1, 0), up(0, 1, 0) {}
    Camera(Vec3 from, Vec3 to, Vec3 up) : from(from), to(to), up(up) {}
};