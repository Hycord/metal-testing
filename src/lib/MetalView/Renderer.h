#pragma once
#include "../../config.h"
#include "./MeshFactory.h"

class Renderer
{
public:
    Renderer(MTL::Device *device);
    ~Renderer();
    void draw(MTK::View *view);

private:
    void buildMeshes();
    void buildShaders();
    MTL::RenderPipelineState *buildShader(const char *fileName, const char *vertexName, const char *fragmentName);
    MTL::Device *device;
    MTL::CommandQueue *commandQueue;
    MTL::Buffer *triangleMesh;
    MTL::RenderPipelineState *trianglePipeline, *generalPipeline;
    Mesh quadMesh;
    float t;
};