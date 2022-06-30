#include <glm/gtc/type_ptr.hpp>

#include <Magpie/mat.h>

using namespace Magpie;

Mat4 Matrix::Perspective(float fov, float aspect, float near, float far) {
    return glm::perspective(fov, aspect, near, far);
}

Mat4 Matrix::LookAt(Vec3 eye, Vec3 center, Vec3 up) {
    glm::vec3 e (eye.x, eye.y, eye.z);
    glm::vec3 c (center.x, center.y, center.z);
    glm::vec3 u (up.x, up.y, up.z);
    return glm::lookAt(e,c,u);
}

Mat4 Matrix::Inverse(Mat4 matrix) {
    return glm::inverse(matrix);
}

float* Matrix::ValuePtr(Mat4 matrix) {
    return glm::value_ptr(matrix);
}

Vec4 operator*(Magpie::Mat4 lhs, Magpie::Vec4 rhs) {
    glm::vec4 v (rhs.x, rhs.y, rhs.z, rhs.w);
    glm::vec4 result = lhs * v;
    return Vec4(result.x, result.y, result.z, result.w);
}