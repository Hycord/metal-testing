#pragma once

#include "engine/components/renderables/primitives/2d/TextBoxPrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include <memory>
#include <string>

class UITextBoxPrimitive : public RenderablePrimitive {
public:
    UITextBoxPrimitive(MTL::Device* device,
                      const std::string& text,
                      const std::string& fontPath,
                      float fontSize,
                      const TextBoxConfig& config = TextBoxConfig{});
    
    UITextBoxPrimitive(MTL::Device* device,
                      const std::string& text,
                      float width, float height,
                      const std::string& fontPath,
                      float fontSize,
                      const TextBoxConfig& config = TextBoxConfig{});
    
    ~UITextBoxPrimitive();
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setText(const std::string& text);
    void setFontSize(float size);
    void setTextAlignment(TextAlign align);
    void setTextJustification(TextJustify justify);
    void setBackgroundColor(const simd::float4& color);
    void setTextColor(const simd::float4& color);
    void setPadding(float left, float right, float top, float bottom);
    
    const std::string& getText() const;
    float getWidth() const { return transform.getWidth(); }
    float getHeight() const { return transform.getHeight(); }
    
    UITransform& getTransform() { return transform; }
    const UITransform& getTransform() const { return transform; }
    
    void getContentSize(float& width, float& height) const override;

private:
    void updatePrimitivePosition();
    void onColorChanged() override;
    
    MTL::Device* device;
    UITransform transform;
    std::shared_ptr<TextBoxPrimitive> textBox;
    bool autoSizeToContent;
};
