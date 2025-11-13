#pragma once
#include "config.h"
#include "factories/MeshFactory.h"

class PipelineFactory
{
public:
    PipelineFactory(MTL::Device *device);

    ~PipelineFactory();

    void set_filename(const char *filename);

    void set_vertex_entry_point(const char *entryPoint);

    void set_fragment_entry_point(const char *entryPoint);

    void set_vertex_descriptor(MTL::VertexDescriptor *descriptor);

    MTL::RenderPipelineState *build();

private:
    const char *fileName, *vertexEntryPoint, *fragmentEntryPoint;
    MTL::VertexDescriptor *vertexDescriptor = nullptr;
    MTL::Device *device;
};