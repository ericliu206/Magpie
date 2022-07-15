#pragma once

#include "mat.h"
#include "scene.h"
#include "angle.h"

#include <string>

namespace Magpie {
    class PathTracer {
        public:
            virtual ~PathTracer() {};
            virtual void Initialize() {};
            virtual void SetSky(std::string filename) = 0;
            virtual void SetDimensions(unsigned int width, unsigned int height);
            virtual void SetViewMatrix(Mat4 matrix);
            virtual void LoadScene(Scene scene) = 0;
            virtual void Render() = 0;
            virtual float* GetPixels();
        protected:
            unsigned int width = 800, height = 600;
            Mat4 view;
            Mat4 projection = Matrix::Perspective(Radians(45.0f), (float)width / height, 0.1f, 100.0f);
            float* pixels;
    };
}