#include "components/renderables/primitives/2d/TextBoxPrimitive.h"
#include "core/LogManager.h"

TextBoxPrimitive::TextBoxPrimitive(MTL::Device *device,
                                   const std::string &text,
                                   float x, float y,
                                   float width, float height,
                                   const std::string &fontPath,
                                   float fontSize,
                                   const TextBoxConfig &config)
    : device(device),
      fontPath(fontPath),
      x(x), y(y),
      width(width), height(height),
      fontSize(fontSize),
      config(config)
{
    transform.setPosition(x, y);
    transform.setSize(width, height);
    
    background = std::make_shared<RoundedRectanglePrimitive>(
        device,
        x, y,
        width, height,
        config.cornerRadius,
        config.backgroundColor,
        config.cornerSegments
    );
    
    float textX = x + config.paddingLeft;
    float textY = y + config.paddingTop;
    float textWidth = width - config.paddingLeft - config.paddingRight;
    float textHeight = height - config.paddingTop - config.paddingBottom;
    
    textPrimitive = std::make_shared<TextPrimitive>(
        device,
        text,
        textX, textY,
        fontPath,
        fontSize,
        config.textColor
    );
    
    textPrimitive->setBoxSize(textWidth, textHeight);
    textPrimitive->setAlignment(TextAlign::Start);
    textPrimitive->setJustification(TextJustify::Start);
    
    if (config.enableWordWrap) {
        textPrimitive->setWrap(true);
    }
}

void TextBoxPrimitive::draw(MTL::RenderCommandEncoder *encoder,
                             const simd::float4x4 &projection,
                             const simd::float4x4 &view)
{
    simd::float2 pos = transform.getAbsolutePosition();
    if (pos.x != x || pos.y != y) {
        x = pos.x;
        y = pos.y;
        updateLayout();
    }
    
    if (background) {
        background->draw(encoder, projection, view);
    }
    
    if (textPrimitive) {
        textPrimitive->draw(encoder, projection, view);
    }
}

void TextBoxPrimitive::setText(const std::string &text)
{
    if (textPrimitive) {
        textPrimitive->setText(text);
    }
}

void TextBoxPrimitive::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
    transform.setPosition(newX, newY);
    updateLayout();
}

void TextBoxPrimitive::setSize(float newWidth, float newHeight)
{
    width = newWidth;
    height = newHeight;
    transform.setSize(newWidth, newHeight);
    updateLayout();
}

void TextBoxPrimitive::setFontSize(float size)
{
    fontSize = size;
    if (textPrimitive) {
        textPrimitive->setFontSize(size);
    }
}

void TextBoxPrimitive::setTextAlignment(TextAlign align)
{
    if (textPrimitive) {
        textPrimitive->setAlignment(align);
    }
}

void TextBoxPrimitive::setTextJustification(TextJustify justify)
{
    if (textPrimitive) {
        textPrimitive->setJustification(justify);
    }
}

void TextBoxPrimitive::setBackgroundColor(const simd::float4 &color)
{
    config.backgroundColor = color;
    if (background) {
        background->setColor(color);
    }
}

void TextBoxPrimitive::setTextColor(const simd::float4 &color)
{
    config.textColor = color;
    if (textPrimitive) {
        textPrimitive->setColor(color);
    }
}

void TextBoxPrimitive::setPadding(float left, float right, float top, float bottom)
{
    config.paddingLeft = left;
    config.paddingRight = right;
    config.paddingTop = top;
    config.paddingBottom = bottom;
    updateLayout();
}

const std::string &TextBoxPrimitive::getText() const
{
    static const std::string empty = "";
    return textPrimitive ? textPrimitive->getText() : empty;
}

void TextBoxPrimitive::updateLayout()
{
    if (background) {
        background->setPosition(x, y);
        background->setSize(width, height);
    }
    
    if (textPrimitive) {
        float textX = x + config.paddingLeft;
        float textY = y + config.paddingTop;
        float textWidth = width - config.paddingLeft - config.paddingRight;
        float textHeight = height - config.paddingTop - config.paddingBottom;
        
        textPrimitive->setPosition(textX, textY);
        textPrimitive->setBoxSize(textWidth, textHeight);
    }
}

void TextBoxPrimitive::onColorChanged()
{
}

void TextBoxPrimitive::getContentSize(float& width, float& height) const
{
    if (!textPrimitive) {
        width = config.paddingLeft + config.paddingRight;
        height = config.paddingTop + config.paddingBottom;
        return;
    }

    float textWidth, textHeight;
    textPrimitive->getContentSize(textWidth, textHeight);
    
    width = textWidth + config.paddingLeft + config.paddingRight;
    height = textHeight + config.paddingTop + config.paddingBottom;
    
    if (width < config.paddingLeft + config.paddingRight) {
        width = config.paddingLeft + config.paddingRight;
    }
    if (height < config.paddingTop + config.paddingBottom) {
        height = config.paddingTop + config.paddingBottom;
    }
}
