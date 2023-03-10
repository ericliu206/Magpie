#pragma once

#include <glm/mat4x4.hpp>

#include "vec.h"

namespace Magpie {
    typedef glm::mat4 Mat4;
    
    namespace Matrix {
        Mat4 Perspective(float fov, float aspect, float near, float far);
        Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up);
        Mat4 Inverse(Mat4 matrix);
        float* ValuePtr(Mat4 matrix);
    }
}

Magpie::Vec4 operator*(Magpie::Mat4 lhs, Magpie::Vec4 rhs);