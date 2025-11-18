#pragma once
#include "engine/config.h"
#include "engine/components/engine/Material.h"

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

    void setDepthBias(float bias, float slopeScale = 0.0f) { 
        depthBias = bias; 
        depthBiasSlopeScale = slopeScale;
    }
    float getDepthBias() const { return depthBias; }

    Material* getMaterial() { return material; }
    const Material* getMaterial() const { return material; }

    void updateMesh(const Mesh &m);

private:
    Mesh mesh;
    Material *material;
    simd::float4x4 transform;
    bool screenSpace;
    MTL::PrimitiveType primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
    float depthBias = 0.0f;
    float depthBiasSlopeScale = 0.0f;
};
