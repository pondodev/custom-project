#include "vec2.h"

Vec2 Vec2::normalised() {
    if ( x == 0 && y == 0 ) return *this; // catch vectors of length 0

    return *this / magnitude();
}

float Vec2::magnitude() {
    return std::sqrt( x*x + y*y );
}

Vec2 Vec2::abs() {
    Vec2 v;

    v.x = std::abs( x );
    v.y = std::abs( y );

    return v;
}

float Vec2::dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 Vec2::operator+(const Vec2& other) {
    Vec2 v;

    v.x = this->x + other.x;
    v.y = this->y + other.y;

    return v;
}

Vec2 Vec2::operator+(const float other) {
    Vec2 v;

    v.x = this->x + other;
    v.y = this->y + other;

    return v;
}

Vec2 Vec2::operator-(const Vec2& other) {
    Vec2 v;

    v.x = this->x - other.x;
    v.y = this->y - other.y;

    return v;
}

Vec2 Vec2::operator-(const float other) {
    Vec2 v;

    v.x = this->x - other;
    v.y = this->y - other;

    return v;
}

Vec2 Vec2::operator*(const float other) {
    Vec2 v;

    v.x = this->x * other;
    v.y = this->y * other;

    return v;
}

Vec2 Vec2::operator/(const float other) {
    Vec2 v;

    v.x = this->x / other;
    v.y = this->y / other;

    return v;
}

Vec2& Vec2::operator+=(const Vec2& other) {
    *this = *this + other;
    return *this;
}

Vec2& Vec2::operator+=(const float other) {
    *this = *this + other;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other) {
    *this = *this - other;
    return *this;
}

Vec2& Vec2::operator-=(const float other) {
    *this = *this - other;
    return *this;
}

Vec2& Vec2::operator*=(const float other) {
    *this = *this * other;
    return *this;
}

Vec2 Vec2::operator/=(const float other) {
    *this = *this / other;
    return *this;
}

bool Vec2::operator==(const Vec2& other) {
    return  this->x == other.x &&
            this->y == other.y;
}

bool Vec2::operator!=(const Vec2& other) {
    return !(*this == other);
}
