#include <yaml-cpp/yaml.h>

#include <Magpie/scene.h>

using namespace Magpie;

// Convert between Vec4 and YAML
namespace YAML {
    template<>
    struct convert<Vec4> {
        static Node encode(const Vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, Vec4& rhs) {
            if(!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}

Scene Magpie::LoadSceneFromFile(std::string file) {
    Scene scene;
    YAML::Node sceneData = YAML::LoadFile(file);
    scene.skyFilename = sceneData["sky"].as<std::string>();
    YAML::Node sphereData = sceneData["spheres"];
    for (std::size_t i = 0; i < sphereData.size(); i++) {
        Vec4 sphere = sphereData[i].as<Vec4>();
        scene.AddSphere(Vec3(sphere.x, sphere.y, sphere.z), sphere.w);
    }
    return scene;
}

void Scene::AddSphere(Vec3 center, float radius) {
    spheres.push_back(Vec4(center.x, center.y, center.z, radius));
}

const std::vector<Vec4>& Scene::GetSpheres() {
    return spheres;
}