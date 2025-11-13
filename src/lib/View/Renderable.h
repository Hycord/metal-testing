#pragma once
#include "../../config.h"
#include "Material.h"

class Renderable {
public:
    Renderable(const Mesh &mesh, Material *material);
    ~Renderable();

    void setTransform(const simd::float4x4 &t) { transform = t; }
    const simd::float4x4 &getTransform() const { return transform; }

    void draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &projection, const simd::float4x4 &view);

private:
    Mesh mesh;
    Material *material;
    simd::float4x4 transform;
};
