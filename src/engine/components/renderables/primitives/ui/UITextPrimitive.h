#pragma once

#include "engine/components/renderables/primitives/2d/TextPrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include <memory>
#include <string>

class UITextPrimitive : public RenderablePrimitive {
public:
    UITextPrimitive(MTL::Device* device,
                   const std::string& text,
                   const std::string& fontPath,
                   float fontSize,
                   const simd::float4& color);
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setText(const std::string& text);
    void setFontSize(float size);
    void setFont(const std::string& fontPath);
    void setBoxSize(float width, float height);
    void clearBoxSize();
    void setAlignment(TextAlign align);
    void setJustification(TextJustify justify);
    void setWrap(bool enabled);
    
    const std::string& getText() const;
    float getFontSize() const;
    void measureText(float& width, float& height) const;
    
    UITransform& getTransform() { return transform; }
    const UITransform& getTransform() const { return transform; }
    
    void getContentSize(float& width, float& height) const override;

private:
    void updatePrimitivePosition();
    void onColorChanged() override;
    
    MTL::Device* device;
    UITransform transform;
    std::shared_ptr<TextPrimitive> textPrimitive;
};
