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

typedef struct {
    float3 a;
    float3 b;
    float3 c;
} Triangle;

RayHit create_ray_hit() {
    RayHit hit;
    hit.position = (float3)(0.0f, 0.0f, 0.0f);
    hit.distance = INFINITY;
    hit.normal = (float3)(0.0f, 0.0f, 0.0f);
    return hit;
}

void intersect_ground_plane(Ray ray, RayHit* hit) {
    float t = -ray.origin.y / ray.direction.y;
    if (t > 0 && t < hit->distance) {
        hit->distance = t;
        hit->position = ray.origin + t * ray.direction;
        hit->normal = (float3)(0.0f, 1.0f, 0.0f);
    }
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

int intersect_triangle(Ray ray, float3 vert0, float3 vert1, float3 vert2, float* t) {
    float3 edge1 = vert1 - vert0;
    float3 edge2 = vert2 - vert0;
    float3 pvec = cross(ray.direction, edge2);
    float det = dot(edge1, pvec);
    float inv_det = 1.0f / det;
    float3 tvec = ray.origin - vert0;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0 || u > 1.0f)
        return 0;
    float3 qvec = cross(tvec, edge1);
    float v = dot(ray.direction, qvec) * inv_det;
    if (v < 0.0 || u + v > 1.0f)
        return 0;
    *t = dot(edge2, qvec) * inv_det;
    return 1;
}

RayHit trace(Ray ray, int ground, __global float4* spheres, int numSpheres, __global Triangle* triangles, int numTriangles) {
    RayHit bestHit = create_ray_hit();
    if (ground) intersect_ground_plane(ray, &bestHit);
    for (int i = 0; i < numSpheres; i++) {
        intersect_sphere(ray, &bestHit, spheres[i]);
    }
    for (int i = 0; i < numTriangles; i++) {
        float t;
        if (intersect_triangle(ray, triangles[i].a, triangles[i].b, triangles[i].c, &t)) {
            if (t > 0 && t < bestHit.distance) {
                bestHit.distance = t;
                bestHit.position = ray.origin + t * ray.direction;
                bestHit.normal = normalize(cross(triangles[i].b - triangles[i].a, triangles[i].c - triangles[i].a));
            }
        }
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
                       __global Triangle* triangles, 
                       int ground, 
                       float4 directionalLight, 
                       int skyWidth, 
                       int skyHeight,
                       int numSpheres, 
                       int numTriangles)
{
    int gid = get_global_id(0);

    Ray r = rays[gid];
    frame[gid] = (float4)(0.0f);
    for (int i = 0; i < 8; i++) {
        RayHit hit = trace(r, ground, spheres, numSpheres, triangles, numTriangles);
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
    raytrace = new cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(program, "raytrace");
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
    raytrace->getKernel().setArg(7, skyWidth);
    raytrace->getKernel().setArg(8, skyHeight);
    skyBuffer = new cl::Buffer(*context, skyData.begin(), skyData.end(), true);
    stbi_image_free(data);
}

void OpenCLPathTracer::LoadScene(Scene scene){
    if (!scene.skyFilename.empty()) {
        SetSky(scene.skyFilename);
    }

    raytrace->getKernel().setArg(5, scene.ground);
    raytrace->getKernel().setArg(6, Vec4(scene.directionalLight.direction.x, 
                                         scene.directionalLight.direction.y, 
                                         scene.directionalLight.direction.z, 
                                         scene.directionalLight.intensity));

    const std::vector<Vec4>& spheres = scene.GetSpheres();
    raytrace->getKernel().setArg(9, spheres.size());
    sphereBuffer = new cl::Buffer(*context, spheres.begin(), spheres.end() , true);
    
    const std::vector<Vec3>& triangles = scene.GetTriangles();
    // in OpenCL, 3-component vector data types are aligned to a 4 * sizeof(component) boundary
    std::vector<Vec4> triangleData(triangles.size());
    for (int i = 0; i < triangles.size(); i++) {
        triangleData[i] = Vec4(triangles[i].x ,triangles[i].y ,triangles[i].z , 0.0f);
    }
    raytrace->getKernel().setArg(10, triangleData.size());
    triangleBuffer = new cl::Buffer(*context, triangleData.begin(), triangleData.end() , true);
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
    (*raytrace)(cl::EnqueueArgs(*queue, cl::NDRange(width*height)), rayBuffer, *skyBuffer, *deviceFrame, *sphereBuffer, *triangleBuffer);
    cl::copy(*queue, *deviceFrame, frame.begin(), frame.end());
    pixels = (float*)frame.data();
}