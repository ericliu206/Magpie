#include <Magpie/vec.h>

#include <math.h>

using namespace Magpie;

Vec3::Vec3() {}

Vec3::Vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3::Vec3(const Vec3& other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}

Vec3& Vec3::operator+=(const Vec3& rhs) {
    this->x+=rhs.x;
    this->y+=rhs.y;
    this->z+=rhs.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& rhs) {
    this->x-=rhs.x;
    this->y-=rhs.y;
    this->z-=rhs.z;
    return *this;
}

Vec3 operator+(Vec3 lhs, const Vec3& rhs) {
    lhs += rhs;
    return lhs;
}

Vec3 operator-(Vec3 lhs, const Vec3& rhs) {
    lhs -= rhs;
    return lhs;
}

Vec3 operator*(Vec3 lhs, float rhs) {
    return Vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Vec3 operator/(Vec3 lhs, float rhs) {
    return Vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

Vec3 operator*(float lhs, Vec3 rhs) {
    return Vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

float Vector::Magnitude(Vec3 v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vec3 Vector::Normalize(Vec3 v) {
    float m = Vector::Magnitude(v);
    return v/m;
}

Vec3 Vector::Cross(Vec3 a, Vec3 b) {
    Vec3 product;
    product.x = a.y*b.z - a.z*b.y;
    product.y = a.z*b.x - a.x*b.z;
    product.z = a.x*b.y - a.y*b.x;
    return product;
}