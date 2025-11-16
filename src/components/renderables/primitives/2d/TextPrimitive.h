#pragma once

#include "components/engine/Renderable.h"
#include "components/renderables/primitives/RenderablePrimitive.h"
#include <memory>
#include <string>
#include <vector>

class Font;

enum class TextAlign {
    Start,
    Center,
    End
};

enum class TextJustify {
    Start,
    Center,
    End
};


class TextPrimitive : public RenderablePrimitive
{
public:
    TextPrimitive(MTL::Device *device, 
                  const std::string &text, 
                  float x, float y, 
                  const std::string &fontPath,
                  float fontSize, 
                  const simd::float4 &col);
    
    void draw(MTL::RenderCommandEncoder *encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;
    
    void setText(const std::string &text);
    void setPosition(float x, float y);
    void setFontSize(float size);
    void setFont(const std::string &fontPath);
    
    
    void setBoxSize(float width, float height);
    void clearBoxSize(); 
    
    
    void setAlignment(TextAlign align); 
    void setJustification(TextJustify justify); 
    
    
    void setWrap(bool enabled);
    
    const std::string &getText() const { return text; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getFontSize() const { return fontSize; }
    
    
    void measureText(float& width, float& height) const;

    void getContentSize(float& width, float& height) const override;

private:
    void rebuild();
    void ensureMesh();
    std::vector<std::string> wrapText(const std::string& text, float maxWidth) const;
    
    MTL::Device *device;
    std::string text;
    std::string fontPath;
    float x, y;
    float fontSize;
    bool dirty;
    
    
    bool hasBoxSize;
    float boxWidth;
    float boxHeight;
    TextAlign alignment;
    TextJustify justification;
    bool wrapEnabled;
    
    std::shared_ptr<Font> font;
    Mesh mesh{};
    std::shared_ptr<Renderable> renderable;
    
    void onColorChanged() override;
};
