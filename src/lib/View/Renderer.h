#pragma once
#include "../../config.h"
#include "./MeshFactory.h"

class Renderer
{
public:
    Renderer(MTL::Device *device, CA::MetalLayer *metalLayer);
    ~Renderer();
    void draw(const simd::float4x4 &view);

private:
    void buildMeshes();
    void buildShaders();
    MTL::RenderPipelineState *buildShader(const char *fileName, const char *vertexName, const char *fragmentName);
    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    CA::MetalDrawable *drawableArea;
    MTL::CommandQueue *commandQueue;

    
    MTL::Buffer *triangleMesh;
    MTL::RenderPipelineState *trianglePipeline, *generalPipeline;
    Mesh quadMesh;
    float t;
};