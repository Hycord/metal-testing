#pragma once

#include "engine/components/renderables/core/WorldContainer.h"
#include "engine/config.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include <memory>
#include <vector>

class WorldElement : public WorldContainer {
public:
    WorldElement(MTL::Device* device);
    virtual ~WorldElement();

    virtual void drawPrimitives(MTL::RenderCommandEncoder* encoder,
                               const simd::float4x4& projection,
                               const simd::float4x4& view);

    void addPrimitive(const std::shared_ptr<RenderablePrimitive>& prim);
    void clearPrimitives();

    void setPosition(float x, float y, float z);
    simd::float3 getPosition() const { return position; }
    
    void setRotation(float pitch, float yaw, float roll);
    simd::float3 getRotation() const { return rotation; }

    void getContentSize(float& width, float& height) const;

    void render(MTL::RenderCommandEncoder* encoder,
                const simd::float4x4& projection,
                const simd::float4x4& view) override;

protected:
    MTL::Device* device;
    simd::float3 position{0.0f, 0.0f, 0.0f};
    simd::float3 rotation{0.0f, 0.0f, 0.0f};
    std::vector<std::shared_ptr<RenderablePrimitive>> primitives;
    
    MTL::Device* getDevice() const { return device; }
};
