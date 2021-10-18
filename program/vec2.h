#ifndef VEC2_H
#define VEC2_H

#include <ctgmath>

struct Vec2 {
    float x;
    float y;

    Vec2 normalised();
    float magnitude();
    Vec2 abs();

    static float dot(Vec2 a, Vec2 b);

    Vec2 operator+(const Vec2& other);
    Vec2 operator+(const float other);
    Vec2 operator-(const Vec2& other);
    Vec2 operator-(const float other);
    Vec2 operator*(const float other);
    Vec2 operator/(const float other);
    Vec2& operator+=(const Vec2& other);
    Vec2& operator+=(const float other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator-=(const float other);
    Vec2& operator*=(const float other);
    Vec2 operator/=(const float other);
    bool operator==(const Vec2& other);
    bool operator!=(const Vec2& other);
};

#endif
