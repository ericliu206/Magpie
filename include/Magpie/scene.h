#pragma once

#include "light.h"
#include "vec.h"
#include "material.h"

#include <string>
#include <vector>

namespace Magpie {
    struct Sphere {
        Vec3 center;
        float radius;
        int materialIndex;
    };

    struct Triangle {
        Vec3 a;
        Vec3 b;
        Vec3 c;
        int materialIndex;
    };

    class Scene {
        public:
            std::string skyFilename;
            bool ground;
            DirectionalLight directionalLight;
            const std::vector<Sphere>& GetSpheres();
            const std::vector<Triangle>& GetTriangles();
            const std::vector<Material>& GetMaterials();
            void AddSphere(Vec3 center, float radius, int materialIndex);
            void AddTriangle(Vec3 a, Vec3 b, Vec3 c, int materialIndex);
            void AddMaterial(Material material);
        private:
            std::vector<Sphere> spheres;
            std::vector<Triangle> triangles;
            std::vector<Material> materials;
    };
    Scene LoadSceneFromFile(std::string file);
}