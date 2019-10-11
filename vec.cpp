#include "vec.h"

void vec3::normalize() {
    f32 length_inv = 1.0f / length();

    xyz[0] *= length_inv;
    xyz[1] *= length_inv;
    xyz[2] *= length_inv;
}

vec3 operator+(const vec3 &a, const vec3 &b) {
    return vec3{a.xyz[0] + b.xyz[0], a.xyz[1] + b.xyz[1], a.xyz[2] + b.xyz[2]};
}

vec3 operator-(const vec3 &a, const vec3 &b) {
    return vec3{a.xyz[0] - b.xyz[0], a.xyz[1] - b.xyz[1], a.xyz[2] - b.xyz[2]};
}

vec3 operator*(const vec3 &a, const vec3 &b) {
    return vec3{a.xyz[0] * b.xyz[0], a.xyz[1] * b.xyz[1], a.xyz[2] * b.xyz[2]};
}

vec3 operator/(const vec3 &a, const vec3 &b) {
    return vec3{a.xyz[0] / b.xyz[0], a.xyz[1] / b.xyz[1], a.xyz[2] / b.xyz[2]};
}

vec3 operator*(const vec3 &a, f32 scalar) {
    return vec3{a.xyz[0] * scalar, a.xyz[1] * scalar, a.xyz[2] * scalar};
}

vec3 operator*(f32 scalar, const vec3 &a) {
    return vec3{a.xyz[0] * scalar, a.xyz[1] * scalar, a.xyz[2] * scalar};
}

vec3 operator/(const vec3 &a, f32 scalar) {
    return vec3{a.xyz[0] / scalar, a.xyz[1] / scalar, a.xyz[2] / scalar};
}

vec3 operator/=(const vec3 &a, f32 scalar) {
    return vec3{a.xyz[0] / scalar, a.xyz[1] / scalar, a.xyz[2] / scalar};
}

vec3 &vec3::operator+=(const vec3 &that) {
    this->xyz[0] += that.xyz[0];
    this->xyz[1] += that.xyz[1];
    this->xyz[2] += that.xyz[2];
    return *this;
}

vec3 &vec3::operator-=(const vec3 &that) {
    this->xyz[0] -= that.xyz[0];
    this->xyz[1] -= that.xyz[1];
    this->xyz[2] -= that.xyz[2];
    return *this;
}

vec3 &vec3::operator*=(const vec3 &that) {
    this->xyz[0] *= that.xyz[0];
    this->xyz[1] *= that.xyz[1];
    this->xyz[2] *= that.xyz[2];
    return *this;
}

vec3 &vec3::operator/=(const vec3 &that) {
    this->xyz[0] /= that.xyz[0];
    this->xyz[1] /= that.xyz[1];
    this->xyz[2] /= that.xyz[2];
    return *this;
}

// vec2 stuff

void vec2::normalize() {
    f32 length_inv = 1.0f / length();

    xy[0] *= length_inv;
    xy[1] *= length_inv;
}

vec2 operator+(const vec2 &a, const vec2 &b) {
    return vec2(a.xy[0] + b.xy[0], a.xy[1] + b.xy[1]);
}

vec2 operator-(const vec2 &a, const vec2 &b) {
    return vec2(a.xy[0] - b.xy[0], a.xy[1] - b.xy[1]);
}

vec2 operator*(const vec2 &a, const vec2 &b) {
    return vec2(a.xy[0] * b.xy[0], a.xy[1] * b.xy[1]);
}

vec2 operator/(const vec2 &a, const vec2 &b) {
    return vec2(a.xy[0] / b.xy[0], a.xy[1] / b.xy[1]);
}

vec2 operator*(const vec2 &a, f32 scalar) {
    return vec2(a.xy[0] * scalar, a.xy[1] * scalar);
}

vec2 operator*(f32 scalar, const vec2 &a) {
    return vec2(a.xy[0] * scalar, a.xy[1] * scalar);
}

vec2 operator/(const vec2 &a, f32 scalar) {
    return vec2(a.xy[0] / scalar, a.xy[1] / scalar);
}

vec2 operator/=(const vec2 &a, f32 scalar) {
    return vec2(a.xy[0] / scalar, a.xy[1] / scalar);
}

vec2 &vec2::operator+=(const vec2 &that) {
    this->xy[0] += that.xy[0];
    this->xy[1] += that.xy[1];
    return *this;
}

vec2 &vec2::operator-=(const vec2 &that) {
    this->xy[0] -= that.xy[0];
    this->xy[1] -= that.xy[1];
    return *this;
}

vec2 &vec2::operator*=(const vec2 &that) {
    this->xy[0] *= that.xy[0];
    this->xy[1] *= that.xy[1];
    return *this;
}

vec2 &vec2::operator/=(const vec2 &that) {
    this->xy[0] /= that.xy[0];
    this->xy[1] /= that.xy[1];
    return *this;
}
