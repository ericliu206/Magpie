#pragma once

#include "light.h"
#include "vec.h"
#include "material.h"

#include <string>
#include <vector>

namespace Magpie {
    class Scene {
        public:
            std::string skyFilename;
            bool ground;
            DirectionalLight directionalLight;
            const std::vector<Vec4>& GetSpheres();
            const std::vector<Vec3>& GetTriangles();
            const std::vector<Material>& GetMaterials();
            void AddSphere(Vec3 center, float radius);
            void AddTriangle(Vec3 a, Vec3 b, Vec3 c);
            void AddMaterial(Material material);
        private:
            std::vector<Vec4> spheres;
            std::vector<Vec3> triangles;
            std::vector<Material> materials;
    };
    Scene LoadSceneFromFile(std::string file);
}