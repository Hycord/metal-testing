#pragma once
#include "config.h"
#include "components/engine/Shader.h"

class Material {
public:
    Material(Shader* shader);
    ~Material();

    void setColor(const simd::float4 &c) { color = c; }
    const simd::float4 &getColor() const { return color; }

    void setTexture(MTL::Texture *tex);
    MTL::Texture *getTexture() const { return texture; }

    void setSampler(MTL::SamplerState *samplerState);
    MTL::SamplerState *getSampler() const { return sampler; }

    void apply(MTL::RenderCommandEncoder *encoder);

private:
    void ensureDefaultSampler();
    
    Shader* shader;
    simd::float4 color;
    MTL::Texture *texture = nullptr;
    MTL::SamplerState *sampler = nullptr;
};
