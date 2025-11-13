#pragma once
#include "../../config.h"
#include "Renderable.h"
#include <vector>

enum class ProjectionMode {
    Perspective,
    Orthographic,
    Custom
};

class MeshRenderer {
public:
    MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer);
    ~MeshRenderer();

    void addRenderable(Renderable *r);
    void draw(const simd::float4x4 &view);

    void setProjectionMode(ProjectionMode mode) { projectionMode = mode; }
    void setCustomProjection(const simd::float4x4 &m) { customProjection = m; projectionMode = ProjectionMode::Custom; }
    void setOrthoParams(float left, float right, float bottom, float top, float near, float far);

private:
    simd::float4x4 computeProjection();

    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    CA::MetalDrawable *drawableArea;
    MTL::CommandQueue *commandQueue;

    std::vector<Renderable*> renderables;
    ProjectionMode projectionMode;
    simd::float4x4 customProjection;
    // orthographic params
    float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
};
