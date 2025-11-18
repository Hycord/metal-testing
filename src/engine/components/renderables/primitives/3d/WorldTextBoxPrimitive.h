#pragma once

#include "engine/components/renderables/primitives/2d/TextBoxPrimitive.h"
#include <memory>
#include <string>

class WorldTextBoxPrimitive : public RenderablePrimitive {
public:
    WorldTextBoxPrimitive(MTL::Device* device,
                         const std::string& text,
                         float x, float y, float z,
                         float worldWidth, float worldHeight,
                         const std::string& fontPath,
                         const TextBoxConfig& config = TextBoxConfig{});
    
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;
    
    void setText(const std::string& text);
    void setTextAlignment(TextAlign align);
    void setTextJustification(TextJustify justify);
    void setBackgroundColor(const simd::float4& color);
    void setTextColor(const simd::float4& color);
    void setPadding(float left, float right, float top, float bottom);
    void setPosition(float x, float y, float z);
    void setWorldSize(float width, float height);
    void setOrientation(const simd::float3& forward, const simd::float3& up);
    
    const std::string& getText() const;
    void getContentSize(float& width, float& height) const override;
    
    simd::float3 getPosition() const { return position; }
    simd::float3 getForward() const { return forward; }
    simd::float3 getUp() const { return up; }

private:
    void updateTransform();
    void onColorChanged() override;
    void onTransformChanged() override;
    
    MTL::Device* device;
    simd::float3 position;
    simd::float3 forward;
    simd::float3 up;
    float worldWidth;
    float worldHeight;
    std::shared_ptr<TextBoxPrimitive> textBox;
    float calculatedFontSize;
};
