#include "Material.h"
#include "../LogManager/LogManager.h"

Material::Material(Shader* shader) : shader(shader)
{
    // set default color
    color = {1.0f, 1.0f, 1.0f, 1.0f};
}

Material::~Material() {}

void Material::apply(MTL::RenderCommandEncoder *encoder)
{
    if (!shader) return;
    // set pipeline state
    encoder->setRenderPipelineState(shader->pipeline());

    // For now we only have a color slot reserved at fragment buffer(0) in future shaders.
    // Not all shaders will use it; it's safe to set it here.
    encoder->setFragmentBytes(&color, sizeof(simd::float4), 0);
}
