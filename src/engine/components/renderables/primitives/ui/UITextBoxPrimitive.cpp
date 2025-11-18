#include "engine/components/renderables/primitives/ui/UITextBoxPrimitive.h"
#include "engine/core/LogManager.h"

UITextBoxPrimitive::~UITextBoxPrimitive()
{
    LOG_DESTROY("UITextBoxPrimitive");
}

UITextBoxPrimitive::UITextBoxPrimitive(MTL::Device *device,
                                       const std::string& text,
                                       const std::string& fontPath,
                                       float fontSize,
                                       const TextBoxConfig& config)
    : device(device),
      autoSizeToContent(config.autoSizeToContent)
{
    LOG_CONSTRUCT("UITextBoxPrimitive");
    
    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        text,
        0.0f, 0.0f,
        fontPath,
        fontSize,
        config
    );
    
    float width, height;
    textBox->getContentSize(width, height);
    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

UITextBoxPrimitive::UITextBoxPrimitive(MTL::Device* device,
                                       const std::string& text,
                                       float width, float height,
                                       const std::string& fontPath,
                                       float fontSize,
                                       const TextBoxConfig& config)
    : device(device),
      autoSizeToContent(false)
{
    LOG_CONSTRUCT("UITextBoxPrimitive");
    
    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        text,
        0.0f, 0.0f,
        width, height,
        fontPath,
        fontSize,
        config
    );
    
    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

void UITextBoxPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                               const simd::float4x4& projection,
                               const simd::float4x4& view)
{
    updatePrimitivePosition();
    
    if (textBox) {
        textBox->draw(encoder, projection, view);
    }
}

void UITextBoxPrimitive::setText(const std::string& text)
{
    if (textBox) {
        textBox->setText(text);
        
        if (autoSizeToContent) {
            float width, height;
            textBox->getContentSize(width, height);
            transform.setSize(width, height);
        }
    }
}

void UITextBoxPrimitive::setFontSize(float size)
{
    if (textBox) {
        textBox->setFontSize(size);
    }
}

void UITextBoxPrimitive::setTextAlignment(TextAlign align)
{
    if (textBox) {
        textBox->setTextAlignment(align);
    }
}

void UITextBoxPrimitive::setTextJustification(TextJustify justify)
{
    if (textBox) {
        textBox->setTextJustification(justify);
    }
}

void UITextBoxPrimitive::setBackgroundColor(const simd::float4& color)
{
    if (textBox) {
        textBox->setBackgroundColor(color);
    }
}

void UITextBoxPrimitive::setTextColor(const simd::float4& color)
{
    if (textBox) {
        textBox->setTextColor(color);
    }
}

void UITextBoxPrimitive::setPadding(float left, float right, float top, float bottom)
{
    if (textBox) {
        textBox->setPadding(left, right, top, bottom);
    }
}

const std::string& UITextBoxPrimitive::getText() const
{
    static std::string empty;
    return textBox ? textBox->getText() : empty;
}

void UITextBoxPrimitive::getContentSize(float& width, float& height) const
{
    width = transform.getWidth();
    height = transform.getHeight();
}

void UITextBoxPrimitive::updatePrimitivePosition()
{
    simd::float2 pos = transform.getAbsolutePosition();
    textBox->setPosition(pos.x, pos.y);
}

void UITextBoxPrimitive::onColorChanged()
{
}
