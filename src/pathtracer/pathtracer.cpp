#include <Magpie/pathtracer.h>

using namespace Magpie;

void PathTracer::SetDimensions(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

void PathTracer::SetViewMatrix(Mat4 matrix) {
    this->view = matrix;
}

float* PathTracer::GetPixels() {
    return pixels;
}