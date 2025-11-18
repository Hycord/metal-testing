#pragma once
#include "engine/config.h"
#include "engine/factories/PipelineFactory.h"
#include <string>

class Shader {
public:
    Shader(MTL::Device *device,
           const std::string &filename,
           const std::string &vertexEntry,
           const std::string &fragmentEntry,
           MTL::VertexDescriptor* vertexDescriptor = nullptr,
           bool enableAlphaBlending = false);
    ~Shader();

    MTL::RenderPipelineState* pipeline() { return pipelineState; }
    MTL::Device* getDevice() const { return device; }

private:
    MTL::Device *device;
    MTL::RenderPipelineState *pipelineState;
    bool alphaBlending;
};
