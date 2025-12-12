#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <print>
#include <format>
#include <algorithm>

#define SQ(x) ((x) * (x))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

template<typename T>
static T lerp(const T a, const T b, float t) {
    return a * (1.0f - t) + (b * t);
}

class Vec3 {
public:
    union
    {
        float v[3];
        struct { float x, y, z; };
    };

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& pVec) const {
        return Vec3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]);
    }

    Vec3 operator+(const float scalar) const {
        return Vec3(v[0] + scalar, v[1] + scalar, v[2] + scalar);
    }

    Vec3& operator+=(const Vec3& pVec) {
        v[0] += pVec.v[0];
        v[1] += pVec.v[1];
        v[2] += pVec.v[2];
        return *this;
    }

    Vec3& operator+=(const float scalar) {
        v[0] += scalar;
        v[1] += scalar;
        v[2] += scalar;
        return *this;
    }

    Vec3 operator*(const Vec3& pVec) const {
        return Vec3(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2]);
    }

    Vec3 operator*(const float scalar) const {
        return Vec3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
    }

    Vec3& operator*=(const Vec3& pVec) {
        v[0] *= pVec.v[0];
        v[1] *= pVec.v[1];
        v[2] *= pVec.v[2];
        return *this;
    }

    Vec3& operator*=(const float scalar) {
        v[0] *= scalar;
        v[1] *= scalar;
        v[2] *= scalar;
        return *this;
    }

    Vec3 operator/(const Vec3& pVec) const {
        return Vec3(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2]);
    }

    Vec3 operator/(const float scalar) const {
        return Vec3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
    }

    Vec3& operator/=(const Vec3& pVec) {
        v[0] /= pVec.v[0];
        v[1] /= pVec.v[1];
        v[2] /= pVec.v[2];
        return *this;
    }

    Vec3& operator/=(const float scalar) {
        v[0] /= scalar;
        v[1] /= scalar;
        v[2] /= scalar;
        return *this;
    }

    Vec3 operator-(const Vec3& pVec) const {
        return Vec3(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2]);
    }

    Vec3 operator-(const float scalar) const {
        return Vec3(v[0] - scalar, v[1] - scalar, v[2] - scalar);
    }

    Vec3& operator-=(const Vec3& pVec) {
        v[0] -= pVec.v[0];
        v[1] -= pVec.v[1];
        v[2] -= pVec.v[2];
        return *this;
    }

    Vec3& operator-=(const float scalar) {
        v[0] -= scalar;
        v[1] -= scalar;
        v[2] -= scalar;
        return *this;
    }

    Vec3 operator-() const {
        return Vec3(-v[0], -v[1], -v[2]);
    }

    float length() const {
        return std::sqrt(SQ(x) + SQ(y) + SQ(z));
    }

    float lengthSquared() const {
        return SQ(x) + SQ(y) + SQ(z);
    }

    Vec3 normalize(void) const {
        float len = 1.0f / sqrtf(SQ(x) + SQ(y) + SQ(z));
        return Vec3(x * len, y * len, z * len);
    }

    float normalize_GetLength() {
        float length = sqrtf(SQ(x) + SQ(y) + SQ(z));
        float len = 1.0f / length;
        v[0] *= len; v[1] *= len; v[2] *= len;
        return length;
    }

    float dot(const Vec3& pVec) const {
        return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
    }

    Vec3 cross(const Vec3& v1) const {
        return Vec3(v1.v[1] * v[2] - v1.v[2] * v[1], v1.v[2] * v[0] - v1.v[0] * v[2], v1.v[0] * v[1] - v1.v[1] * v[0]);
    }

    float Max() const {
        return max(max(x, y), z);
    }

    float Min() const {
        return min(min(x, y), z);
    }
};

float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
};

Vec3 Max(const Vec3& v1, const Vec3& v2) {
    return Vec3(
        max(v1.x, v2.x),
        max(v1.y, v2.y),
        max(v1.z, v2.z)
    );
};

Vec3 Min(const Vec3& v1, const Vec3& v2) {
    return Vec3(
        min(v1.x, v2.x),
        min(v1.y, v2.y),
        min(v1.z, v2.z)
    );
};

template <>
struct std::formatter<Vec3> : std::formatter<float> {
    auto format(const Vec3& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec3[{}, {}, {}]", v.x, v.y, v.z);
    }
};

class Vec4 {
public:
    union
    {
        float v[4];
        struct { float x, y, z, w; };
    };

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vec4 operator+(const Vec4& pVec) const {
        return Vec4(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2], v[3] + pVec.v[3]);
    }

    Vec4 operator+(const float scalar) const {
        return Vec4(v[0] + scalar, v[1] + scalar, v[2] + scalar, v[3] + scalar);
    }

    Vec4& operator+=(const Vec4& pVec) {
        v[0] += pVec.v[0];
        v[1] += pVec.v[1];
        v[2] += pVec.v[2];
        v[3] += pVec.v[3];
        return *this;
    }

    Vec4& operator+=(const float scalar) {
        v[0] += scalar;
        v[1] += scalar;
        v[2] += scalar;
        v[3] += scalar;
        return *this;
    }

    Vec4 operator*(const Vec4& pVec) const {
        return Vec4(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2], v[3] * pVec.v[3]);
    }

    Vec4 operator*(const float scalar) const {
        return Vec4(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
    }

    Vec4& operator*=(const Vec4& pVec) {
        v[0] *= pVec.v[0];
        v[1] *= pVec.v[1];
        v[2] *= pVec.v[2];
        v[3] *= pVec.v[3];
        return *this;
    }

    Vec4& operator*=(const float scalar) {
        v[0] *= scalar;
        v[1] *= scalar;
        v[2] *= scalar;
        v[3] *= scalar;
        return *this;
    }

    Vec4 operator/(const Vec4& pVec) const {
        return Vec4(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2], v[3] / pVec.v[3]);
    }

    Vec4 operator/(const float scalar) const {
        return Vec4(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3] / scalar);
    }

    Vec4& operator/=(const Vec4& pVec) {
        v[0] /= pVec.v[0];
        v[1] /= pVec.v[1];
        v[2] /= pVec.v[2];
        v[3] /= pVec.v[3];
        return *this;
    }

    Vec4& operator/=(const float scalar) {
        v[0] /= scalar;
        v[1] /= scalar;
        v[2] /= scalar;
        v[3] /= scalar;
        return *this;
    }

    Vec4 operator-(const Vec4& pVec) const {
        return Vec4(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2], v[3] - pVec.v[3]);
    }

    Vec4 operator-(const float scalar) const {
        return Vec4(v[0] - scalar, v[1] - scalar, v[2] - scalar, v[3] - scalar);
    }

    Vec4& operator-=(const Vec4& pVec) {
        v[0] -= pVec.v[0];
        v[1] -= pVec.v[1];
        v[2] -= pVec.v[2];
        v[3] -= pVec.v[3];
        return *this;
    }

    Vec4& operator-=(const float scalar) {
        v[0] -= scalar;
        v[1] -= scalar;
        v[2] -= scalar;
        v[3] -= scalar;
        return *this;
    }

    Vec4 operator-() const {
        return Vec4(-v[0], -v[1], -v[2], -v[3]);
    }

    float length() const {
        return std::sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
    }

    float lengthSquared() const {
        return SQ(x) + SQ(y) + SQ(z) + SQ(w);
    }

    Vec4 normalize(void) {
        float len = 1.0f / sqrtf(SQ(x) + SQ(y) + SQ(z) + SQ(w));
        return Vec4(x * len, y * len, z * len, w * len);
    }

    float normalize_GetLength() {
        float length = sqrtf(SQ(x) + SQ(y) + SQ(z) + SQ(w));
        float len = 1.0f / length;
        v[0] *= len; v[1] *= len; v[2] *= len; v[3] *= len;
        return length;
    }

    float Max() const {
        return max(max(x, y), max(z, w));
    }

    float Min() const {
        return min(min(x, y), min(z, w));
    }

    Vec4 divideByW() {
        return Vec4(x / w, y / w, z / w, w / w);
    }

    float dot(const Vec4& pVec) const {
        return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2] + v[3] * pVec.v[3];
    }
};

Vec4 Max(const Vec4& v1, const Vec4& v2) {
    return Vec4(
        max(v1.x, v2.x),
        max(v1.y, v2.y),
        max(v1.z, v2.z),
        max(v1.w, v2.w)
    );
};

Vec4 Min(const Vec4& v1, const Vec4& v2) {
    return Vec4(
        min(v1.x, v2.x),
        min(v1.y, v2.y),
        min(v1.z, v2.z),
        min(v1.w, v2.w)
    );
};

template <>
struct std::formatter<Vec4> : std::formatter<float> {
    auto format(const Vec4& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec4[{}, {}, {}, {}]", v.x, v.y, v.z, v.w);
    }
};

class Matrix {
public:
    union
    {
        float a[4][4];
        float m[16];
    };

    Matrix() {
        setIdentity();
    }

    Matrix(float m[16]) {
        for (int i = 0; i < 16; i++) {
            this->m[i] = m[i];
        }
    }

    float& operator[](int index) {
        return m[index];
    }

    void setIdentity() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                a[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    Vec4 mul(const Vec4& v) {
        return Vec4(
            v.x * m[0] + v.y * m[1] + v.z * m[2] + v.w * m[3],
            v.x * m[4] + v.y * m[5] + v.z * m[6] + v.w * m[7],
            v.x * m[8] + v.y * m[9] + v.z * m[10] + v.w * m[11],
            v.x * m[12] + v.y * m[13] + v.z * m[14] + v.w * m[15]
        );
    }

    Vec3 mulPoint(const Vec3& v) {
        Vec3 v1 = Vec3(
            (v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
            (v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
            (v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]
        );
        return v1;
    }

    void setRotationX(float angle) {
        setIdentity();
        float radians = (3.14159f / 180.0f) * angle;
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        m[5] = cos;
        m[6] = -sin;
        m[9] = sin;
        m[10] = cos;
    }

    void setRotationY(float angle) {
        setIdentity();
        float radians = (3.14159f / 180.0f) * angle;
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        m[0] = cos;
        m[2] = sin;
        m[8] = -sin;
        m[10] = cos;
    }

    void setRotationZ(float angle) {
        setIdentity();
        float radians = (3.14159f / 180.0f) * angle;
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        m[0] = cos;
        m[1] = -sin;
        m[4] = sin;
        m[5] = cos;
    }

    void setTranslation(float tx, float ty, float tz) {
        setIdentity();
        m[3] = tx;
        m[7] = ty;
        m[11] = tz;
    }

    static Matrix setTranslation(const Vec3& t) {
        Matrix result;
        result.m[3] = t.x;
        result.m[7] = t.y;
        result.m[11] = t.z;
        return result;
    }

    void setScaling(float sx, float sy, float sz) {
        setIdentity();
        m[0] = sx;
        m[5] = sy;
        m[10] = sz;
    }

    void setScaling(float s) {
        setIdentity();
        m[0] = s;
        m[5] = s;
        m[10] = s;
    }

    static Matrix setScaling(const Vec3& s) {
        Matrix result;
        result.m[0] = s.x;
        result.m[5] = s.y;
        result.m[10] = s.z;
        return result;
    }

    Vec3 rotateX(float angle, const Vec3& v) {
        setRotationX(angle);
        return mulPoint(v);
    }

    Vec3 rotateY(float angle, const Vec3& v) {
        setRotationY(angle);
        return mulPoint(v);
    }

    Vec3 rotateZ(float angle, const Vec3& v) {
        setRotationZ(angle);
        return mulPoint(v);
    }

    Vec3 translation(float tx, float ty, float tz, const Vec3& v) {
        setTranslation(tx, ty, tz);
        return mulPoint(v);
    }

    Vec3 scaling(float sx, float sy, float sz, const Vec3& v) {
        setScaling(sx, sy, sz);
        return mulPoint(v);
    }

    Matrix mul(const Matrix& matrix) const {
        Matrix ret;
        ret.m[0] = m[0] * matrix.m[0] + m[1] * matrix.m[4] + m[2] * matrix.m[8] + m[3] * matrix.m[12];
        ret.m[1] = m[0] * matrix.m[1] + m[1] * matrix.m[5] + m[2] * matrix.m[9] + m[3] * matrix.m[13];
        ret.m[2] = m[0] * matrix.m[2] + m[1] * matrix.m[6] + m[2] * matrix.m[10] + m[3] * matrix.m[14];
        ret.m[3] = m[0] * matrix.m[3] + m[1] * matrix.m[7] + m[2] * matrix.m[11] + m[3] * matrix.m[15];
        ret.m[4] = m[4] * matrix.m[0] + m[5] * matrix.m[4] + m[6] * matrix.m[8] + m[7] * matrix.m[12];
        ret.m[5] = m[4] * matrix.m[1] + m[5] * matrix.m[5] + m[6] * matrix.m[9] + m[7] * matrix.m[13];
        ret.m[6] = m[4] * matrix.m[2] + m[5] * matrix.m[6] + m[6] * matrix.m[10] + m[7] * matrix.m[14];
        ret.m[7] = m[4] * matrix.m[3] + m[5] * matrix.m[7] + m[6] * matrix.m[11] + m[7] * matrix.m[15];
        ret.m[8] = m[8] * matrix.m[0] + m[9] * matrix.m[4] + m[10] * matrix.m[8] + m[11] * matrix.m[12];
        ret.m[9] = m[8] * matrix.m[1] + m[9] * matrix.m[5] + m[10] * matrix.m[9] + m[11] * matrix.m[13];
        ret.m[10] = m[8] * matrix.m[2] + m[9] * matrix.m[6] + m[10] * matrix.m[10] + m[11] * matrix.m[14];
        ret.m[11] = m[8] * matrix.m[3] + m[9] * matrix.m[7] + m[10] * matrix.m[11] + m[11] * matrix.m[15];
        ret.m[12] = m[12] * matrix.m[0] + m[13] * matrix.m[4] + m[14] * matrix.m[8] + m[15] * matrix.m[12];
        ret.m[13] = m[12] * matrix.m[1] + m[13] * matrix.m[5] + m[14] * matrix.m[9] + m[15] * matrix.m[13];
        ret.m[14] = m[12] * matrix.m[2] + m[13] * matrix.m[6] + m[14] * matrix.m[10] + m[15] * matrix.m[14];
        ret.m[15] = m[12] * matrix.m[3] + m[13] * matrix.m[7] + m[14] * matrix.m[11] + m[15] * matrix.m[15];
        return ret;
    }

    Matrix transpose() const {
        Matrix ret;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ret.a[i][j] = a[j][i];
            }
        }
        return ret;
    }

    Matrix invert() {
        Matrix inv;
        inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
        inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
        inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
        inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
        inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
        inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
        inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
        inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
        inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
        inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
        inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
        inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
        inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
        inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
        inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
        inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];
        if (det == 0) {
            // Handle this case
        }

        det = 1.0 / det;
        for (int i = 0; i < 16; i++) {
            inv.m[i] = inv.m[i] * det;
        }
        return inv;
    }


    void setProjectionMatrix(float zFar, float zNear, float fovDeg, float width, float height) {
        setIdentity();

        float aspect = width / height;
        float fovRad = fovDeg * 0.01745329251f;
        float yScale = 1.0f / tanf(fovRad * 0.5f);
        float xScale = yScale / aspect;

        m[0] = xScale;
        m[5] = yScale;

        m[10] = zFar / (zFar - zNear);
        m[14] = 1.0f;
        m[11] = (-zNear * zFar) / (zFar - zNear);
        m[15] = 0.0f;
    }

    void setLookatMatrix(const Vec3& from, const Vec3& to, const Vec3& up) {
        setIdentity();
        Vec3 z = (to - from).normalize();
        Vec3 x = up.cross(z).normalize();
        Vec3 y = z.cross(x);

        m[0] = x.x;  m[1] = x.y;  m[2] = x.z;
        m[4] = y.x;  m[5] = y.y;  m[6] = y.z;
        m[8] = z.x;  m[9] = z.y;  m[10] = z.z;

        m[3] = -from.dot(x);
        m[7] = -from.dot(y);
        m[11] = -from.dot(z);
    }

};

template <>
struct std::formatter<Matrix> : std::formatter<float> {
    auto format(const Matrix& m, auto& ctx) const {
        return std::format_to(
            ctx.out(),
            "Matrix[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]",
            m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5], m.m[6], m.m[7], m.m[8], m.m[9], m.m[10], m.m[11], m.m[12], m.m[13], m.m[14], m.m[15]);
    }
};

class ShadingFrame {
public:
    Vec3 tangent;
    Vec3 bitangent;
    Vec3 normal;

    ShadingFrame() : tangent(1, 0, 0), bitangent(0, 1, 0), normal(0, 0, 1) {}
    ShadingFrame(const Vec3& normal) {
        this->normal = normal.normalize();
        build();
    }

    void build() {
        Vec3 helper = (std::fabs(normal.x) > 0.99f) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
        tangent = (helper - normal * helper.dot(normal)).normalize();
        bitangent = normal.cross(tangent);
    }

    Vec3 worldToLocal(const Vec3& v) const {
        return Vec3{
            v.dot(tangent),
            v.dot(bitangent),
            v.dot(normal)
        };
    }

    Vec3 localToWorld(const Vec3& v) const {
        return tangent * v.x + bitangent * v.y + normal * v.z;
    }
};

class Quaternion {
public:
    union
    {
        float q[4];
        struct { float a, b, c, d; };
    };

    Quaternion() : a(0), b(0), c(0), d(1) {}
    Quaternion(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}

    Quaternion operator-() const {
        return Quaternion(-a, -b, -c, -d);
    }

    float magnitude() const {
        return sqrt(SQ(a) + SQ(b) + SQ(c) + SQ(d));
    }

    Quaternion normalize() const {
        float mag = magnitude();
        if (mag == 0) {
            // Handle zero magnitude case
        }
        return Quaternion(a / mag, b / mag, c / mag, d / mag);
    }

    Quaternion conjugate() {
        return Quaternion(-a, -b, -c, d);
    }

    Quaternion inverse() {
        float magSq = SQ(a) + SQ(b) + SQ(c) + SQ(d);
        if (magSq == 0) {
            // Handle zero magnitude case
        }
        Quaternion conj = conjugate();
        return Quaternion(conj.a / magSq, conj.b / magSq, conj.c / magSq, conj.d / magSq);
    }

    Quaternion mul(const Quaternion& q2) const {
        return Quaternion(
            (d * q2.a) + (a * q2.d) + (b * q2.c) - (c * q2.b),
            (d * q2.b) - (a * q2.c) + (b * q2.d) + (c * q2.a),
            (d * q2.c) + (a * q2.b) - (b * q2.a) + (c * q2.d),
            (d * q2.d) - (a * q2.a) - (b * q2.b) - (c * q2.c)
        );
    }

    Quaternion fromAxisAngle(Vec3& axis, float angle) {
        Vec3 normAxis = axis.normalize();
        float sinHalf = std::sin(angle * 0.5f);
        float cosHalf = std::cos(angle * 0.5f);
        return Quaternion(
            normAxis.x * sinHalf,
            normAxis.y * sinHalf,
            normAxis.z * sinHalf,
            cosHalf
        );
    }

    float dot(const Quaternion& q2) const {
        return (a * q2.a) + (b * q2.b) + (c * q2.c) + (d * q2.d);
    }

    Quaternion slerp(const Quaternion& q, float t) {
        Quaternion q1 = this->normalize();
        Quaternion q2 = q.normalize();

        float dotProd = q1.dot(q2);

        if (dotProd < 0.0f) {
            q2 = -q2;
            dotProd = -dotProd;
        }

        const float DOT_THRESHOLD = 0.9995f;
        if (dotProd > DOT_THRESHOLD) {
            Quaternion result = Quaternion(
                lerp(q1.a, q2.a, t),
                lerp(q1.b, q2.b, t),
                lerp(q1.c, q2.c, t),
                lerp(q1.d, q2.d, t)
            );
            return result.normalize();
        }

        float theta = std::acos(dotProd);
        float sinTheta = std::sin(theta);

        float thetaT = theta * t;
        float sinThetaT = std::sin(thetaT);

        float s0 = std::sin((1.0f - t) * theta) / sinTheta;
        float s1 = sinThetaT / sinTheta;
        return Quaternion(
            (s0 * q1.a) + (s1 * q2.a),
            (s0 * q1.b) + (s1 * q2.b),
            (s0 * q1.c) + (s1 * q2.c),
            (s0 * q1.d) + (s1 * q2.d)
        );
    }

    Matrix toMatrix() {
        float aa = a * a, ab = a * b, ac = a * c;
        float bb = b * b, bc = b * c, cc = c * c;
        float da = d * a, db = d * b, dc = d * c;
        Matrix m;
        m[0] = 1 - 2 * (bb + cc);
        m[1] = 2 * (ab - dc);
        m[2] = 2 * (ac + db);
        m[3] = 0; m[4] = 2 * (ab + dc);
        m[5] = 1 - 2 * (aa + cc);
        m[6] = 2 * (bc - da);
        m[7] = 0;
        m[8] = 2 * (ac - db);
        m[9] = 2 * (bc + da);
        m[10] = 1 - 2 * (aa + bb);
        m[11] = 0;
        m[12] = m[13] = m[14] = 0;
        m[15] = 1;
        return m;
    }
};

class SphericalCoordZ {
public:
    float radius;
    float theta;
    float phi;

    SphericalCoordZ() : radius(1.0f), theta(0.0f), phi(0.0f) {}
    SphericalCoordZ(float radius, float theta, float phi) : radius(radius), theta(theta), phi(phi) {}
    SphericalCoordZ(const Vec3& v) {
        radius = 1.0f;
        theta = thetaFromCartesian(v);
        phi = phiFromCartesian(v);
    }

    float thetaFromCartesian(const Vec3& v) const {
        return std::acos(v.z / radius);
    }

    float phiFromCartesian(const Vec3& v) const {
        return std::atan(v.y / v.x);
    }

    Vec3 toCartesian() const {
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);
        return Vec3(
            radius * sinTheta * cosPhi,
            radius * sinTheta * sinPhi,
            radius * cosTheta
        );
    }
};

class SphericalCoordY {
public:
    float radius;
    float theta;
    float phi;

    SphericalCoordY() : radius(1.0f), theta(0.0f), phi(0.0f) {}
    SphericalCoordY(float radius, float theta, float phi) : radius(radius), theta(theta), phi(phi) {}
    SphericalCoordY(const Vec3& v) {
        radius = 1.0f;
        theta = thetaFromCartesian(v);
        phi = phiFromCartesian(v);
    }

    float thetaFromCartesian(const Vec3& v) const {
        return std::acos(v.y / radius);
    }

    float phiFromCartesian(const Vec3& v) const {
        return std::atan(v.z / v.x);
    }

    Vec3 toCartesian() const {
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);
        return Vec3(
            radius * sinTheta * cosPhi,
            radius * cosTheta,
            radius * sinTheta * sinPhi
        );
    }
};

class Colour {
public:
    union
    {
        float c[4];
        struct { float r, g, b, a; };
    };

    Colour() : r(0), g(0), b(0), a(1) {}
    Colour(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) :
        r(static_cast<float>(r) / 255.0f),
        g(static_cast<float>(g) / 255.0f),
        b(static_cast<float>(b) / 255.0f),
        a(static_cast<float>(a) / 255.0f) {
    }

    Colour operator+(const Colour& colour) const {
        return Colour(
            min(r + colour.r, 255.0f),
            min(g + colour.g, 255.0f),
            min(b + colour.b, 255.0f),
            min(a + colour.a, 1.0f)
        );
    }

    Colour operator*(const Colour& colour) const {
        return Colour(
            min(r * colour.r, 255.0f),
            min(g * colour.g, 255.0f),
            min(b * colour.b, 255.0f),
            min(a * colour.a, 1.0f)
        );
    }

    Colour operator*(const float a) const {
        return Colour(
            min(r * a, 255.0f),
            min(g * a, 255.0f),
            min(b * a, 255.0f),
            min(this->a * a, 1.0f)
        );
    }

    Colour operator/(const float a) const {
        return Colour(
            min(r / a, 255.0f),
            min(g / a, 255.0f),
            min(b / a, 255.0f),
            min(this->a / a, 1.0f)
        );
    }

};