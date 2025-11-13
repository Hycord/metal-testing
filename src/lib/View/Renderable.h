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

    void setScreenSpace(bool v) { screenSpace = v; }
    bool isScreenSpace() const { return screenSpace; }

    void setPrimitiveType(MTL::PrimitiveType t) { primitiveType = t; }
    MTL::PrimitiveType getPrimitiveType() const { return primitiveType; }

private:
    Mesh mesh;
    Material *material;
    simd::float4x4 transform;
    bool screenSpace;
    MTL::PrimitiveType primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
};

