#include "engine/components/engine/Shader.h"
#include "engine/core/LogManager.h"

Shader::Shader(MTL::Device *device,
               const std::string &filename,
               const std::string &vertexEntry,
               const std::string &fragmentEntry,
               MTL::VertexDescriptor* vertexDescriptor,
               bool enableAlphaBlending)
    : device(device), pipelineState(nullptr), alphaBlending(enableAlphaBlending)
{
    LOG_START("Shader: building pipeline file='%s' vs='%s' fs='%s' alpha=%s",
              filename.c_str(), vertexEntry.c_str(), fragmentEntry.c_str(), enableAlphaBlending ? "true" : "false");
    PipelineFactory builder(device);
    builder.set_vertex_descriptor(vertexDescriptor);
    builder.set_filename(filename.c_str());
    builder.set_vertex_entry_point(vertexEntry.c_str());
    builder.set_fragment_entry_point(fragmentEntry.c_str());
    builder.set_alpha_blending(alphaBlending);
    pipelineState = builder.build();
    LOG_FINISH("Shader: pipeline built state=%p file='%s'", pipelineState, filename.c_str());
}

Shader::~Shader()
{
    LOG_DESTROY("Shader");
    if (pipelineState) pipelineState->release();
}
