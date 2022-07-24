#pragma once

#include "vec.h"

#include <string>
#include <vector>

namespace Magpie {
    class Scene {
        public:
            std::string skyFilename;
            const std::vector<Vec4>& GetSpheres();
            const std::vector<Vec3>& GetTriangles();
            void AddSphere(Vec3 center, float radius);
            void AddTriangle(Vec3 a, Vec3 b, Vec3 c);
        private:
            std::vector<Vec4> spheres;
            std::vector<Vec3> triangles;
    };
    Scene LoadSceneFromFile(std::string file);
}