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

    namespace Vector {
        float Magnitude(Vec3 v);
        Vec3 Normalize(Vec3 v);
        Vec3 Cross(Vec3 a, Vec3 b);
    }
}

Magpie::Vec3 operator+(Magpie::Vec3 lhs, const Magpie::Vec3& rhs);
Magpie::Vec3 operator-(Magpie::Vec3 lhs, const Magpie::Vec3& rhs);
Magpie::Vec3 operator*(Magpie::Vec3 lhs, float rhs);
Magpie::Vec3 operator/(Magpie::Vec3 lhs, float rhs);
Magpie::Vec3 operator*(float lhs, Magpie::Vec3 rhs);