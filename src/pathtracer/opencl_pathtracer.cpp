#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <opencl.hpp>
#include <stb_image.h>

#include <Magpie/pathtracer.h>

using namespace Magpie;

const char* kernelSource = R"cl(
typedef struct {
    float3 origin;
    float3 direction;
    float3 energy;
} Ray;

typedef struct {
    float3 position;
    float distance;
    float3 normal;
} RayHit;

RayHit create_ray_hit() {
    RayHit hit;
    hit.position = (float3)(0.0f, 0.0f, 0.0f);
    hit.distance = INFINITY;
    hit.normal = (float3)(0.0f, 0.0f, 0.0f);
    return hit;
}

void intersect_sphere(Ray ray, RayHit* hit, float4 sphere) {
    float3 d = ray.origin - sphere.xyz;
    float p1 = -dot(ray.direction, d);
    float p2sqr = p1 * p1 - dot(d, d) + sphere.w * sphere.w;
    if (p2sqr < 0)
        return;
    float p2 = sqrt(p2sqr);
    float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;
    if (t > 0 && t < hit->distance) {
        hit->distance = t;
        hit->position = ray.origin + t * ray.direction;
        hit->normal = normalize(hit->position - sphere.xyz);
    }
}

RayHit trace(Ray ray, __global float4* spheres, int numSpheres) {
    RayHit bestHit = create_ray_hit();
    for (int i = 0; i < numSpheres; i++) {
        intersect_sphere(ray, &bestHit, spheres[i]);
    }
    return bestHit;
}

float3 reflect(float3 i, float3 n) {
    return i - 2 * n * dot(i, n);
}

float4 shade(__global const float4* sky, int skyWidth, int skyHeight, Ray* r, RayHit hit) {
    if (hit.distance < INFINITY) {
        float3 specular = (float3)(0.75f, 0.75f, 0.75f);
        r->origin = hit.position + hit.normal * 0.001f;
        r->direction = reflect(r->direction, hit.normal);
        r->energy *= specular;
        return (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    } else {
        r->energy = (float3)(0.0f);
        float x = 0.5f * atan2pi(r->direction.x, r->direction.z);
        float y = acospi(-r->direction.y);
        int row = (int)(y*skyHeight);
        int col = (int)(x*skyWidth);
        return sky[row*skyWidth + col];
    }
}

__kernel void raytrace(__global const Ray* rays, 
                       __global const float4* sky, 
                       __global float4* frame, 
                       __global float4* spheres, 
                       int skyWidth, 
                       int skyHeight,
                       int numSpheres)
{
    int gid = get_global_id(0);

    Ray r = rays[gid];
    frame[gid] = (float4)(0.0f);
    for (int i = 0; i < 8; i++) {
        RayHit hit = trace(r, spheres, numSpheres);
        frame[gid] += (float4)(r.energy, 1.0f) * shade(sky, skyWidth, skyHeight, &r, hit);
        if (r.energy.r == 0.0f || r.energy.g == 0.0f || r.energy.b == 0.0f) {
            break;
        }
    }
}
)cl";

OpenCLPathTracer::~OpenCLPathTracer() {
    delete sphereBuffer;
    delete deviceFrame;
    delete skyBuffer;
    delete raytrace;
    delete queue;
    delete context;
}

void OpenCLPathTracer::Initialize(){
    frame = std::vector<float>(width*height*4);
    context = new cl::Context(CL_DEVICE_TYPE_DEFAULT);
    queue = new cl::CommandQueue(*context);
    cl::Program program(*context, kernelSource , true);
    raytrace = new cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(program, "raytrace");
    deviceFrame = new cl::Buffer(*context, CL_MEM_READ_WRITE, sizeof(float) * width*height*4);
}

void OpenCLPathTracer::SetSky(std::string filename){
    int skyWidth, skyHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &skyWidth, &skyHeight, &nrChannels, 0);
    std::vector<Vec4> skyData(skyHeight*skyWidth);
    for (int i = 0; i < (skyWidth*skyHeight); i++) {
        int pos = i*nrChannels;
        skyData[i] = Vec4(*(data+pos)/255.0f, *(data+pos+1)/255.0f, *(data+pos+2)/255.0f, 1.0f);
    }
    raytrace->getKernel().setArg(4, skyWidth);
    raytrace->getKernel().setArg(5, skyHeight);
    skyBuffer = new cl::Buffer(*context, skyData.begin(), skyData.end(), true);
    stbi_image_free(data);
}

void OpenCLPathTracer::LoadScene(Scene scene){
    if (!scene.skyFilename.empty()) {
        SetSky(scene.skyFilename);
    }
    const std::vector<Vec4>& spheres = scene.GetSpheres();
    raytrace->getKernel().setArg(6, spheres.size());
    sphereBuffer = new cl::Buffer(*context, spheres.begin(), spheres.end() , true);
}

void OpenCLPathTracer::Render(){
    // in OpenCL, 3-component vector data types are aligned to a 4 * sizeof(component) boundary
    std::vector<Vec4> rays(width*height*3);
    Mat4 cameraToWorld = Matrix::Inverse(view);
    Mat4 inverseProjection = Matrix::Inverse(projection);
    for (int i = 0; i < (width*height*3)-2; i += 3) {
        rays[i] = cameraToWorld * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float u = (((float)((i/3)%width)/width) * 2.0f) - 1.0f;
        float v = (((float)(i/(3*width))/height) * 2.0f) - 1.0f;
        Vec4 direction = inverseProjection * Vec4(u, v, 0.0f, 1.0f);
        direction = cameraToWorld * Vec4(direction.x, direction.y, direction.z, 0.0f);
        rays[i+1] = Vector::Normalize(direction);
        rays[i+2] = Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    }
    cl::Buffer rayBuffer(*context, rays.begin(), rays.end(), true);
    (*raytrace)(cl::EnqueueArgs(*queue, cl::NDRange(width*height)), rayBuffer, *skyBuffer, *deviceFrame, *sphereBuffer);
    cl::copy(*queue, *deviceFrame, frame.begin(), frame.end());
    pixels = (float*)frame.data();
}