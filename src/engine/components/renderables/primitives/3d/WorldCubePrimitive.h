#pragma once

#include "engine/components/engine/Renderable.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include <memory>

class WorldCubePrimitive : public RenderablePrimitive
{
public:
    WorldCubePrimitive(MTL::Device *device, float size, const simd::float4 &col, const simd::float3 &position = simd::float3{0.0f, 0.0f, 0.0f});
    ~WorldCubePrimitive();
    
    void draw(MTL::RenderCommandEncoder *encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;
    
    void setSize(float s)
    {
        size = s;
        dirty = true;
    }
    
    float getSize() const { return size; }
    
    void setPosition(float x, float y, float z)
    {
        position = {x, y, z};
        updateTransform();
    }
    
    const simd::float3& getPosition() const { return position; }

private:
    void rebuild();
    void updateTransform();
    MTL::Device *device;
    float size;
    simd::float3 position;
    bool dirty = true;
    Mesh mesh{};
    std::shared_ptr<Renderable> renderable;
    
    void onColorChanged() override;
    void onTransformChanged() override;
};
