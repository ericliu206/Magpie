#pragma once

#include "vec.h"

namespace Magpie {
    class DirectionalLight {
        public:
            DirectionalLight();
            DirectionalLight(Vec3 direction, float intensity);
            Vec3 direction;
            float intensity;
    };
}