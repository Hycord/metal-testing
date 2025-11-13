#include "Shader.h"
#include "../LogManager/LogManager.h"

Shader::Shader(MTL::Device *device, const std::string &filename, const std::string &vertexEntry, const std::string &fragmentEntry, MTL::VertexDescriptor* vertexDescriptor)
    : device(device->retain()), pipelineState(nullptr)
{
    LOG_START("Shader: building pipeline");
    PipelineFactory builder(device);
    builder.set_vertex_descriptor(vertexDescriptor);
    builder.set_filename(filename.c_str());
    builder.set_vertex_entry_point(vertexEntry.c_str());
    builder.set_fragment_entry_point(fragmentEntry.c_str());
    pipelineState = builder.build();
    LOG_FINISH("Shader: pipeline built");
}

Shader::~Shader()
{
    if (pipelineState) pipelineState->release();
    device->release();
}
