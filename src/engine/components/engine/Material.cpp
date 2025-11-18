#include "engine/components/engine/Material.h"
#include "engine/core/LogManager.h"

Material::Material(Shader* shader) : shader(shader)
{
    
    color = {1.0f, 1.0f, 1.0f, 1.0f};
}

Material::~Material() {
    LOG_DESTROY("Material");
    if (texture) {
        texture->release();
        texture = nullptr;
    }
    if (sampler) {
        sampler->release();
        sampler = nullptr;
    }
    if (shader) {
        delete shader;
        shader = nullptr;
    }
}

void Material::setTexture(MTL::Texture *tex)
{
    if (texture == tex)
        return;
    if (texture)
        texture->release();
    texture = tex;
    if (texture)
        texture->retain();
    
    
    ensureDefaultSampler();
}

void Material::ensureDefaultSampler()
{
    if (!sampler) {
        MTL::SamplerDescriptor *samplerDesc = MTL::SamplerDescriptor::alloc()->init();
        samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
        samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
        samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
        samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);

        MTL::Device *dev = shader ? shader->getDevice() : nullptr;
        if (dev) {
            sampler = dev->newSamplerState(samplerDesc);
            LOG_DEBUG("Material: Created default sampler");
        }
        samplerDesc->release();
    }
}

void Material::setSampler(MTL::SamplerState *samplerState)
{
    if (sampler == samplerState)
        return;
    if (sampler)
        sampler->release();
    sampler = samplerState;
    if (sampler)
        sampler->retain();
}

void Material::apply(MTL::RenderCommandEncoder *encoder)
{
    if (!shader) return;
    
    
    ensureDefaultSampler();
    
    
    encoder->setRenderPipelineState(shader->pipeline());

    
    
    encoder->setFragmentBytes(&color, sizeof(simd::float4), 0);

    if (texture)
        encoder->setFragmentTexture(texture, 0);
    if (sampler)
        encoder->setFragmentSamplerState(sampler, 0);
    LOG_DEBUG("Material::apply pipeline=%p tex=%p sampler=%p color=(%.2f,%.2f,%.2f,%.2f)",
              shader->pipeline(), texture, sampler, color.x, color.y, color.z, color.w);
}
