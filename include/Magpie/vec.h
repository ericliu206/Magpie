#pragma once

namespace Magpie {
    class Vec3 {
        public:
            Vec3();
            Vec3(float x, float y, float z);
            Vec3(const Vec3& other);
            float x;
            float y;
            float z;
            Vec3& operator+=(const Vec3& rhs);
            Vec3& operator-=(const Vec3& rhs);
    };

    class Vec4 {
        public:
            Vec4();
            Vec4(float x, float y, float z, float w);
            Vec4(const Vec4& other);
            float x;
            float y;
            float z;
            float w;
            Vec4& operator+=(const Vec4& rhs);
            Vec4& operator-=(const Vec4& rhs);
    };

    namespace Vector {
        float Magnitude(Vec3 v);
        float Magnitude(Vec4 v);
        Vec3 Normalize(Vec3 v);
        Vec4 Normalize(Vec4 v);
        Vec3 Cross(Vec3 a, Vec3 b);
    }
}

Magpie::Vec3 operator+(Magpie::Vec3 lhs, const Magpie::Vec3& rhs);
Magpie::Vec4 operator+(Magpie::Vec4 lhs, const Magpie::Vec4& rhs);
Magpie::Vec3 operator-(Magpie::Vec3 lhs, const Magpie::Vec3& rhs);
Magpie::Vec4 operator-(Magpie::Vec4 lhs, const Magpie::Vec4& rhs);
Magpie::Vec3 operator*(Magpie::Vec3 lhs, float rhs);
Magpie::Vec4 operator*(Magpie::Vec4 lhs, float rhs);
Magpie::Vec3 operator*(float lhs, Magpie::Vec3 rhs);
Magpie::Vec4 operator*(float lhs, Magpie::Vec4 rhs);
Magpie::Vec3 operator/(Magpie::Vec3 lhs, float rhs);
Magpie::Vec4 operator/(Magpie::Vec4 lhs, float rhs);