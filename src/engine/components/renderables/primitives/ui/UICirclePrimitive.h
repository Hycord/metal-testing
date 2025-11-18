#pragma once

#include "engine/components/renderables/primitives/2d/CirclePrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include <memory>

class UICirclePrimitive : public RenderablePrimitive {
public:
    UICirclePrimitive(MTL::Device* device, float radius, const simd::float4& color, int segments = 32);
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setRadius(float radius);
    void setSegments(int segments);
    
    float getRadius() const { return radius; }
    
    UITransform& getTransform() { return transform; }
    const UITransform& getTransform() const { return transform; }
    
    void getContentSize(float& width, float& height) const override;

private:
    void updatePrimitiveTransform();
    void onColorChanged() override;
    
    MTL::Device* device;
    UITransform transform;
    std::shared_ptr<CirclePrimitive> circle;
    float radius;
};
