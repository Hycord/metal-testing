#pragma once

#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include "engine/components/renderables/primitives/2d/RoundedRectanglePrimitive.h"
#include "engine/components/renderables/primitives/2d/TextPrimitive.h"
#include <memory>
#include <string>

struct TextBoxConfig {
    float paddingLeft = 15.0f;
    float paddingRight = 15.0f;
    float paddingTop = 15.0f;
    float paddingBottom = 15.0f;
    
    float cornerRadius = 12.0f;
    int cornerSegments = 32;
    
    simd::float4 backgroundColor{0.10f, 0.10f, 0.10f, 0.85f};
    simd::float4 textColor{0.9f, 0.9f, 0.9f, 1.0f};
    
    bool enableWordWrap = true;
    bool autoSizeToContent = false;
    float maxWidth = 0.0f;
    float maxHeight = 0.0f;
};

class TextBoxPrimitive : public RenderablePrimitive
{
public:
    TextBoxPrimitive(MTL::Device *device,
                     const std::string &text,
                     float x, float y,
                     const std::string &fontPath,
                     float fontSize,
                     const TextBoxConfig &config = TextBoxConfig{});
    
    TextBoxPrimitive(MTL::Device *device,
                     const std::string &text,
                     float x, float y,
                     float width, float height,
                     const std::string &fontPath,
                     float fontSize,
                     const TextBoxConfig &config);
    
    void draw(MTL::RenderCommandEncoder *encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;
    
    void setText(const std::string &text);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setFontSize(float size);
    
    void setTextAlignment(TextAlign align);
    void setTextJustification(TextJustify justify);
    
    void setBackgroundColor(const simd::float4 &color);
    void setTextColor(const simd::float4 &color);
    
    void setPadding(float left, float right, float top, float bottom);
    void setScreenSpace(bool screenSpace);
    void setTransform(const simd::float4x4& transform);
    
    const std::string &getText() const;
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    void getContentSize(float& width, float& height) const override;

private:
    void updateLayout();
    
    MTL::Device *device;
    std::string fontPath;
    float x, y;
    float width, height;
    float fontSize;
    TextBoxConfig config;
    
    std::shared_ptr<RoundedRectanglePrimitive> background;
    std::shared_ptr<TextPrimitive> textPrimitive;
    
    void onColorChanged() override;
};
