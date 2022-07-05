#pragma once

#include "vec.h"

#include <vector>

namespace Magpie {
    class Scene {
        public:
            const std::vector<Vec4>& GetSpheres();
            void AddSphere(Vec3 center, float radius);
        private:
            std::vector<Vec4> spheres;
    };
}