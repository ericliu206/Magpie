#include <Magpie/scene.h>

using namespace Magpie;

void Scene::AddSphere(Vec3 center, float radius) {
    spheres.push_back(Vec4(center.x, center.y, center.z, radius));
}

const std::vector<Vec4>& Scene::GetSpheres() {
    return spheres;
}