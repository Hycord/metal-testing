#pragma once

#include "engine/components/renderables/primitives/2d/RoundedRectanglePrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include <memory>

class UIRoundedRectanglePrimitive : public RenderablePrimitive {
public:
    UIRoundedRectanglePrimitive(MTL::Device* device,
                               float width, float height,
                               float radius,
                               const simd::float4& color,
                               int qualityPerCorner = 6);
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setSize(float width, float height);
    void setRadius(float radius);
    void setQualityPerCorner(int quality);
    
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
    std::shared_ptr<RoundedRectanglePrimitive> roundedRect;
};
