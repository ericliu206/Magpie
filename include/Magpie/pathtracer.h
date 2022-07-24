#pragma once

#include "mat.h"
#include "scene.h"
#include "angle.h"

#include <string>
#include <vector>

// forward declare OpenCL classes
namespace cl {
    class Context;
    class CommandQueue;
    template<typename... Ts> class KernelFunctor;
    class Buffer;
}

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

    class OpenCLPathTracer : public PathTracer {
        public:
            ~OpenCLPathTracer();
            void Initialize();
            void SetSky(std::string filename);
            void LoadScene(Scene scene);
            void Render();
        private:
            std::vector<float> frame;
            cl::Context* context;
            cl::CommandQueue* queue;
            cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>* raytrace;
            cl::Buffer* skyBuffer;
            cl::Buffer* deviceFrame;
            cl::Buffer* sphereBuffer;
            cl::Buffer* triangleBuffer;
    };
}