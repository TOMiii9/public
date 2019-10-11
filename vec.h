#ifndef VEC3_HDR_
#define VEC3_HDR_

#include "typedefs.h"
#include <array>

#include <cmath>

template <typename Type>
void trap_nan(Type fl) {
    if (isnan(fl)) {
        *(int *)1 = 1;
    }
}

/*
====================
vec3
====================
*/
struct vec3 {
    std::array<f32, 3> xyz;

    vec3(const std::array<f32, 3> &that) {
        this->xyz = that;
    }

    vec3() {
        xyz[0] = 0.0f;
        xyz[1] = 0.0f;
        xyz[2] = 0.0f;
    }

    vec3(f32 x, f32 y, f32 z) {
        xyz[0] = x;
        xyz[1] = y;
        xyz[2] = z;
    }

    inline f32 x() const {
        return xyz[0];
    }
    inline f32 y() const {
        return xyz[1];
    }
    inline f32 z() const {
        return xyz[2];
    }

    inline f32 r() {
        return xyz[0];
    }
    inline f32 g() {
        return xyz[1];
    }
    inline f32 b() {
        return xyz[2];
    }

    inline void operator=(const vec3 &that) {
        this->xyz[0] = that.xyz[0];
        this->xyz[1] = that.xyz[1];
        this->xyz[2] = that.xyz[2];
    }

    inline const vec3 &operator+() const {
        return *this;
    }

    inline const vec3 operator-() const {
        return vec3{-xyz[0], -xyz[1], -xyz[2]};
    }

    inline f32 operator[](i32 index) const {
        return xyz[index];
    }

    inline f32 &operator[](i32 index) {
        return xyz[index];
    }

    vec3 &operator+=(const vec3 &that);
    vec3 &operator-=(const vec3 &that);
    vec3 &operator*=(const vec3 &that);
    vec3 &operator/=(const vec3 &that);

    inline f32 length() const {
        f32 l = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
        /*if (_isnanf(l)) {
            int i = 0;
            i     = i;
        }*/
        return l;
    }

    inline f32 length_squared() const {
        return xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2];
    }

    void normalize();
};

vec3 operator+(const vec3 &a, const vec3 &b);
vec3 operator-(const vec3 &a, const vec3 &b);
vec3 operator*(const vec3 &a, const vec3 &b);
vec3 operator/(const vec3 &a, const vec3 &b);
vec3 operator*(const vec3 &a, f32 scalar);
vec3 operator*(f32 scalar, const vec3 &a);
vec3 operator/(const vec3 &a, f32 scalar);
vec3 operator/=(const vec3 &a, f32 scalar);

inline vec3 vec3_normalize(vec3 v) {
    return v / v.length();
}

inline f32 vec3_dot(const vec3 &a, const vec3 &b) {
    return a.xyz[0] * b.xyz[0] + a.xyz[1] * b.xyz[1] + a.xyz[2] * b.xyz[2];
}

inline vec3 vec3_cross(const vec3 &a, const vec3 &b) {
    return vec3{
        a.xyz[1] * b.xyz[2] - a.xyz[2] * b.xyz[1],
        (-(a.xyz[0] * b.xyz[2] - a.xyz[2] * b.xyz[0])),
        a.xyz[0] * b.xyz[1] - a.xyz[1] * b.xyz[0],
    };
}

/*
** vec2 is untested
*/
struct vec2 {
    std::array<f32, 2> xy;

    vec2(const std::array<f32, 2> &that) {
        this->xy = that;
    }

    vec2() {
        xy[0] = 0.0f;
        xy[1] = 0.0f;
    }

    vec2(f32 x, f32 y) {
        xy[0] = x;
        xy[1] = y;
    }

    inline f32 x() const {
        return xy[0];
    }
    inline f32 y() const {
        return xy[1];
    }

    inline f32 u() {
        return xy[0];
    }
    inline f32 v() {
        return xy[1];
    }

    inline void operator=(const vec2 &that) {
        this->xy = that.xy;
    }

    inline const vec2 &operator+() const {
        return *this;
    }

    inline const vec2 operator-() const {
        return vec2(-xy[0], -xy[1]);
    }

    inline f32 operator[](i32 index) const {
        return xy[index];
    }

    inline f32 &operator[](i32 index) {
        return xy[index];
    }

    vec2 &operator+=(const vec2 &that);
    vec2 &operator-=(const vec2 &that);
    vec2 &operator*=(const vec2 &that);
    vec2 &operator/=(const vec2 &that);

    inline f32 length() const {
        return sqrt(xy[0] * xy[0] + xy[1] * xy[1]);
    }

    inline f32 length_squared() const {
        return xy[0] * xy[0] + xy[1] * xy[1];
    }

    void normalize();
};

vec2 operator+(const vec2 &a, const vec2 &b);
vec2 operator-(const vec2 &a, const vec2 &b);
vec2 operator*(const vec2 &a, const vec2 &b);
vec2 operator/(const vec2 &a, const vec2 &b);
vec2 operator*(const vec2 &a, f32 scalar);
vec2 operator*(f32 scalar, const vec2 &a);
vec2 operator/(const vec2 &a, f32 scalar);
vec2 operator/=(const vec2 &a, f32 scalar);

inline vec2 vec2_normalize(vec2 v) {
    return v / v.length();
}

inline f32 vec2_dot(const vec2 &a, const vec2 &b) {
    return a.xy[0] * b.xy[0] + a.xy[1] * b.xy[1];
}

#endif
