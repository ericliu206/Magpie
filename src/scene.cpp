#include <yaml-cpp/yaml.h>

#include <Magpie/scene.h>

using namespace Magpie;

// Convert between Magpie classes and YAML
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

    template<>
    struct convert<Vec3> {
        static Node encode(const Vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, Vec3& rhs) {
            if(!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Material> {
        static Node encode(const Material& m) {
            Node node;
            node["specular"] = m.specular;
            node["albedo"] = m.albedo;
            return node;
        }

        static bool decode(const Node& node, Material& m) {
            if (!node["specular"] || !node["albedo"]) {
                return false;
            }

            m.specular = node["specular"].as<Vec3>();
            m.albedo = node["albedo"].as<Vec3>();
            return true;
        }
    };
}

Scene Magpie::LoadSceneFromFile(std::string file) {
    Scene scene;
    YAML::Node sceneData = YAML::LoadFile(file);
    scene.skyFilename = sceneData["sky"].as<std::string>();
    scene.ground = sceneData["ground"].as<bool>();
    scene.directionalLight = DirectionalLight(sceneData["lights"]["directional"]["direction"].as<Vec3>(), 
                                              sceneData["lights"]["directional"]["intensity"].as<float>());
    YAML::Node sphereData = sceneData["spheres"];
    for (std::size_t i = 0; i < sphereData.size(); i++) {
        Vec4 sphere = sphereData[i][0].as<Vec4>();
        scene.AddSphere(Vec3(sphere.x, sphere.y, sphere.z), sphere.w, sphereData[i][1].as<int>());
    }
    YAML::Node triangleData = sceneData["triangles"];
    for (std::size_t i = 0; i < triangleData.size(); i++) {
        scene.AddTriangle(triangleData[i][0][0].as<Vec3>(), triangleData[i][0][1].as<Vec3>(), triangleData[i][0][2].as<Vec3>(), 
                          triangleData[i][1].as<int>());
    }
    YAML::Node materialData = sceneData["materials"];
    for (std::size_t i = 0; i < materialData.size(); i++) {
        scene.AddMaterial(materialData[i].as<Material>());
    }
    return scene;
}

void Scene::AddSphere(Vec3 center, float radius, int materialIndex) {
    Sphere s;
    s.center = center;
    s.radius = radius;
    s.materialIndex = materialIndex;
    spheres.push_back(s);
}

void Scene::AddTriangle(Vec3 a, Vec3 b, Vec3 c, int materialIndex) {
    Triangle t;
    t.a = a;
    t.b = b;
    t.c = c;
    t.materialIndex = materialIndex;
    triangles.push_back(t);
}

void Scene::AddMaterial(Material material) {
    materials.push_back(material);
}

const std::vector<Sphere>& Scene::GetSpheres() {
    return spheres;
}

const std::vector<Triangle>& Scene::GetTriangles() {
    return triangles;
}

const std::vector<Material>& Scene::GetMaterials() {
    return materials;
}