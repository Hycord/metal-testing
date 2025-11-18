#include "engine/components/renderables/primitives/ui/UITextPrimitive.h"
#include "engine/core/LogManager.h"

UITextPrimitive::UITextPrimitive(MTL::Device* device,
                                 const std::string& text,
                                 const std::string& fontPath,
                                 float fontSize,
                                 const simd::float4& color)
    : device(device)
{
    LOG_CONSTRUCT("UITextPrimitive");
    
    textPrimitive = std::make_shared<TextPrimitive>(
        device,
        text,
        0.0f, 0.0f,
        fontPath,
        fontSize,
        color
    );
    
    float width, height;
    textPrimitive->getContentSize(width, height);
    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

void UITextPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                           const simd::float4x4& projection,
                           const simd::float4x4& view)
{
    updatePrimitivePosition();
    
    if (textPrimitive) {
        textPrimitive->draw(encoder, projection, view);
    }
}

void UITextPrimitive::setText(const std::string& text)
{
    if (textPrimitive) {
        textPrimitive->setText(text);
        
        float width, height;
        textPrimitive->getContentSize(width, height);
        transform.setSize(width, height);
    }
}

void UITextPrimitive::setFontSize(float size)
{
    if (textPrimitive) {
        textPrimitive->setFontSize(size);
        
        float width, height;
        textPrimitive->getContentSize(width, height);
        transform.setSize(width, height);
    }
}

void UITextPrimitive::setFont(const std::string& fontPath)
{
    if (textPrimitive) {
        textPrimitive->setFont(fontPath);
        
        float width, height;
        textPrimitive->getContentSize(width, height);
        transform.setSize(width, height);
    }
}

void UITextPrimitive::setBoxSize(float width, float height)
{
    if (textPrimitive) {
        textPrimitive->setBoxSize(width, height);
        transform.setSize(width, height);
    }
}

void UITextPrimitive::clearBoxSize()
{
    if (textPrimitive) {
        textPrimitive->clearBoxSize();
        
        float width, height;
        textPrimitive->getContentSize(width, height);
        transform.setSize(width, height);
    }
}

void UITextPrimitive::setAlignment(TextAlign align)
{
    if (textPrimitive) {
        textPrimitive->setAlignment(align);
    }
}

void UITextPrimitive::setJustification(TextJustify justify)
{
    if (textPrimitive) {
        textPrimitive->setJustification(justify);
    }
}

void UITextPrimitive::setWrap(bool enabled)
{
    if (textPrimitive) {
        textPrimitive->setWrap(enabled);
    }
}

const std::string& UITextPrimitive::getText() const
{
    static std::string empty;
    return textPrimitive ? textPrimitive->getText() : empty;
}

float UITextPrimitive::getFontSize() const
{
    return textPrimitive ? textPrimitive->getFontSize() : 0.0f;
}

void UITextPrimitive::measureText(float& width, float& height) const
{
    if (textPrimitive) {
        textPrimitive->measureText(width, height);
    } else {
        width = 0.0f;
        height = 0.0f;
    }
}

void UITextPrimitive::getContentSize(float& width, float& height) const
{
    width = transform.getWidth();
    height = transform.getHeight();
}

void UITextPrimitive::updatePrimitivePosition()
{
    simd::float2 pos = transform.getAbsolutePosition();
    textPrimitive->setPosition(pos.x, pos.y);
}

void UITextPrimitive::onColorChanged()
{
    if (textPrimitive) {
        textPrimitive->setColor(getColor());
    }
}
