#include <Magpie/light.h>

using namespace Magpie;

DirectionalLight::DirectionalLight() {}

DirectionalLight::DirectionalLight(Vec3 direction, float intensity) {
    this->direction = direction;
    this->intensity = intensity;
}