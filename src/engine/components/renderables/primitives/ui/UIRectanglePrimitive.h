#pragma once

#include "engine/components/renderables/primitives/2d/RectanglePrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include <memory>

class UIRectanglePrimitive : public RenderablePrimitive {
public:
    UIRectanglePrimitive(MTL::Device* device, float width, float height, const simd::float4& color);
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setSize(float width, float height);
    float getWidth() const { return transform.getWidth(); }
    float getHeight() const { return transform.getHeight(); }
    
    UITransform& getTransform() { return transform; }
    const UITransform& getTransform() const { return transform; }
    
    void getContentSize(float& width, float& height) const override;

private:
    void updatePrimitiveTransform();
    void onColorChanged() override;
    
    MTL::Device* device;
    UITransform transform;
    std::shared_ptr<RectanglePrimitive> rectangle;
};
