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
    float3 specular;
    float3 albedo;
} RayHit;

typedef struct {
    float3 center;
    float radius;
    int materialIndex;
} Sphere;

typedef struct {
    float3 a;
    float3 b;
    float3 c;
    int materialIndex;
} Triangle;

typedef struct {
    float3 specular;
    float3 albedo;
} Material;

RayHit create_ray_hit() {
    RayHit hit;
    hit.position = (float3)(0.0f, 0.0f, 0.0f);
    hit.distance = INFINITY;
    hit.normal = (float3)(0.0f, 0.0f, 0.0f);
    return hit;
}

void intersect_ground_plane(Ray ray, RayHit* hit, __global Material* materials) {
    float t = -ray.origin.y / ray.direction.y;
    if (t > 0 && t < hit->distance) {
        hit->distance = t;
        hit->position = ray.origin + t * ray.direction;
        hit->normal = (float3)(0.0f, 1.0f, 0.0f);
        hit->specular = materials[0].specular;
        hit->albedo = materials[0].albedo;
    }
}

void intersect_sphere(Ray ray, RayHit* hit, Sphere sphere, __global Material* materials) {
    float3 d = ray.origin - sphere.center;
    float p1 = -dot(ray.direction, d);
    float p2sqr = p1 * p1 - dot(d, d) + sphere.radius * sphere.radius;
    if (p2sqr < 0)
        return;
    float p2 = sqrt(p2sqr);
    float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;
    if (t > 0 && t < hit->distance) {
        hit->distance = t;
        hit->position = ray.origin + t * ray.direction;
        hit->normal = normalize(hit->position - sphere.center);
        hit->specular = materials[sphere.materialIndex].specular;
        hit->albedo = materials[sphere.materialIndex].albedo;
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

RayHit trace(Ray ray, int ground, __global Sphere* spheres, int numSpheres, __global Triangle* triangles, int numTriangles, __global Material* materials) {
    RayHit bestHit = create_ray_hit();
    if (ground) intersect_ground_plane(ray, &bestHit, materials);
    for (int i = 0; i < numSpheres; i++) {
        intersect_sphere(ray, &bestHit, spheres[i], materials);
    }
    for (int i = 0; i < numTriangles; i++) {
        float t;
        if (intersect_triangle(ray, triangles[i].a, triangles[i].b, triangles[i].c, &t)) {
            if (t > 0 && t < bestHit.distance) {
                bestHit.distance = t;
                bestHit.position = ray.origin + t * ray.direction;
                bestHit.normal = normalize(cross(triangles[i].b - triangles[i].a, triangles[i].c - triangles[i].a));
                bestHit.specular = materials[triangles[i].materialIndex].specular;
                bestHit.albedo = materials[triangles[i].materialIndex].albedo;
            }
        }
    }
    return bestHit;
}

float3 reflect(float3 i, float3 n) {
    return i - 2 * n * dot(i, n);
}

float4 shade(__global const float4* sky, int skyWidth, int skyHeight, float4 directionalLight, Ray* r, RayHit hit) {
    if (hit.distance < INFINITY) {
        r->origin = hit.position + hit.normal * 0.001f;
        r->direction = reflect(r->direction, hit.normal);
        r->energy *= hit.specular;
        return (float4)(clamp(dot(hit.normal, directionalLight.xyz) * -1, 0.0f, 1.0f) * directionalLight.w * hit.albedo, 1.0f);
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
                       __global Sphere* spheres, 
                       __global Triangle* triangles, 
                       __global Material* materials, 
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
        RayHit hit = trace(r, ground, spheres, numSpheres, triangles, numTriangles, materials);
        frame[gid] += (float4)(r.energy, 1.0f) * shade(sky, skyWidth, skyHeight, directionalLight, &r, hit);
        if (r.energy.r == 0.0f || r.energy.g == 0.0f || r.energy.b == 0.0f) {
            break;
        }
    }
}
)cl";

// structs to work with OpenCL alignment
struct OpenCLSphere {
    Vec4 center;
    float radius;
    int materialIndex;
    double pad;
};

struct OpenCLTriangle {
    Vec4 a;
    Vec4 b;
    Vec4 c;
    int materialIndex;
    float pad1;
    float pad2;
    float pad3;
};

struct OpenCLMaterial {
    Vec4 specular;
    Vec4 albedo;
};

OpenCLPathTracer::~OpenCLPathTracer() {
    delete materialBuffer;
    delete triangleBuffer;
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
    raytrace = new cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(program, "raytrace");
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
    raytrace->getKernel().setArg(8, skyWidth);
    raytrace->getKernel().setArg(9, skyHeight);
    skyBuffer = new cl::Buffer(*context, skyData.begin(), skyData.end(), true);
    stbi_image_free(data);
}

void OpenCLPathTracer::LoadScene(Scene scene){
    if (!scene.skyFilename.empty()) {
        SetSky(scene.skyFilename);
    }

    raytrace->getKernel().setArg(6, scene.ground);
    raytrace->getKernel().setArg(7, Vec4(scene.directionalLight.direction.x, 
                                         scene.directionalLight.direction.y, 
                                         scene.directionalLight.direction.z, 
                                         scene.directionalLight.intensity));

    const std::vector<Sphere>& spheres = scene.GetSpheres();
    raytrace->getKernel().setArg(10, spheres.size());
    std::vector<OpenCLSphere> sphereData(spheres.size());
    for (int i = 0; i < spheres.size(); i++) {
        Sphere s = spheres[i];
        sphereData[i].center = Vec4(s.center.x, s.center.y, s.center.z, 0.0f);
        sphereData[i].radius = s.radius;
        sphereData[i].materialIndex = s.materialIndex;
    }
    sphereBuffer = new cl::Buffer(*context, sphereData.begin(), sphereData.end() , true);
    
    const std::vector<Triangle>& triangles = scene.GetTriangles();
    std::vector<OpenCLTriangle> triangleData(triangles.size());
    for (int i = 0; i < triangles.size(); i++) {
        Triangle t = triangles[i];
        triangleData[i].a = Vec4(t.a.x ,t.a.y ,t.a.z , 0.0f);
        triangleData[i].b = Vec4(t.b.x ,t.b.y ,t.b.z , 0.0f);
        triangleData[i].c = Vec4(t.c.x ,t.c.y ,t.c.z , 0.0f);
        triangleData[i].materialIndex = t.materialIndex;
    }
    raytrace->getKernel().setArg(11, triangleData.size());
    triangleBuffer = new cl::Buffer(*context, triangleData.begin(), triangleData.end() , true);

    const std::vector<Material>& materials = scene.GetMaterials();
    std::vector<OpenCLMaterial> materialData(materials.size());
    for (int i = 0; i < materials.size(); i++) {
        Material m = materials[i];
        materialData[i].specular = Vec4(m.specular.x, m.specular.y, m.specular.z, 0.0f);
        materialData[i].albedo = Vec4(m.albedo.x, m.albedo.y, m.albedo.z, 0.0f);
    }
    materialBuffer = new cl::Buffer(*context, materialData.begin(), materialData.end(), true);
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
    (*raytrace)(cl::EnqueueArgs(*queue, cl::NDRange(width*height)), rayBuffer, *skyBuffer, *deviceFrame, *sphereBuffer, *triangleBuffer, *materialBuffer);
    cl::copy(*queue, *deviceFrame, frame.begin(), frame.end());
    pixels = (float*)frame.data();
}