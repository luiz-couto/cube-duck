#pragma once
#include <random>

float generateRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(min, max);

    return dist(mt);
}

int generateRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);

    return dist(mt);
}