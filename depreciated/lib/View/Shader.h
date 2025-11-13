#pragma once
#include "../../config.h"
#include "PipelineFactory.h"
#include <string>

class Shader {
public:
    Shader(MTL::Device *device, const std::string &filename, const std::string &vertexEntry, const std::string &fragmentEntry, MTL::VertexDescriptor* vertexDescriptor = nullptr);
    ~Shader();

    MTL::RenderPipelineState* pipeline() { return pipelineState; }

private:
    MTL::Device *device;
    MTL::RenderPipelineState *pipelineState;
};
