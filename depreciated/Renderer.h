#pragma once
#include "../../config.h"

class Renderer
{
public:
    Renderer(MTL::Device *device, CA::MetalLayer *metalLayer);
    ~Renderer();
    void draw(const simd::float4x4 &view);

private:
    void buildMeshes();
    void buildShaders();
    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    CA::MetalDrawable *drawableArea;
    MTL::CommandQueue *commandQueue;


    MTL::Buffer *triangleMesh;
    MTL::RenderPipelineState *trianglePipeline, *generalPipeline;
    Mesh quadMesh;
    float t;
};