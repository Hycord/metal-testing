#pragma once
#include "config.h"
#include "components/engine/Shader.h"

class Material {
public:
    Material(Shader* shader);
    ~Material();

    void setColor(const simd::float4 &c) { color = c; }
    const simd::float4 &getColor() const { return color; }

    void apply(MTL::RenderCommandEncoder *encoder);

private:
    Shader* shader;
    simd::float4 color;
};
