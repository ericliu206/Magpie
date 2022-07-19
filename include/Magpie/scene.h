#pragma once

#include "vec.h"

#include <string>
#include <vector>

namespace Magpie {
    class Scene {
        public:
            std::string skyFilename;
            const std::vector<Vec4>& GetSpheres();
            void AddSphere(Vec3 center, float radius);
        private:
            std::vector<Vec4> spheres;
    };
    Scene LoadSceneFromFile(std::string file);
}