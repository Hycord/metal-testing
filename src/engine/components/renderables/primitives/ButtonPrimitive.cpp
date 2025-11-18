#include "engine/components/renderables/primitives/ButtonPrimitive.h"
#include "engine/core/LogManager.h"

static simd::float4 darken(const simd::float4& c, float f)
{
    return simd::float4{c.x * f, c.y * f, c.z * f, c.w};
}

ButtonPrimitive::ButtonPrimitive(MTL::Device* device,
                                 const std::string& text,
                                 const std::string& fontPath,
                                 float fontSize,
                                 const TextBoxConfig& config)
    : device(device)
{
    LOG_CONSTRUCT("ButtonPrimitive");

    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        text,
        0.0f, 0.0f,
        fontPath,
        fontSize,
        config
    );

    baseBackground = config.backgroundColor;
    hoverBackground = darken(baseBackground, 0.9f);
    pressedBackground = darken(baseBackground, 0.75f);
}

ButtonPrimitive::ButtonPrimitive(MTL::Device* device,
                                 const std::string& text,
                                 float x, float y,
                                 float width, float height,
                                 const std::string& fontPath,
                                 float fontSize,
                                 const TextBoxConfig& config)
    : device(device)
{
    LOG_CONSTRUCT("ButtonPrimitive");

    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        text,
        x, y,
        width, height,
        fontPath,
        fontSize,
        config
    );

    baseBackground = config.backgroundColor;
    hoverBackground = darken(baseBackground, 0.9f);
    pressedBackground = darken(baseBackground, 0.75f);
}

ButtonPrimitive::~ButtonPrimitive()
{
    LOG_DESTROY("ButtonPrimitive");
}

void ButtonPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                           const simd::float4x4& projection,
                           const simd::float4x4& view)
{
    if (textBox) {
        textBox->draw(encoder, projection, view);
    }
}

void ButtonPrimitive::setText(const std::string& text)
{
    if (textBox) {
        textBox->setText(text);
    }
}

const std::string& ButtonPrimitive::getText() const
{
    static std::string empty;
    return textBox ? textBox->getText() : empty;
}

void ButtonPrimitive::setBackgroundColor(const simd::float4& color)
{
    baseBackground = color;
    hoverBackground = darken(baseBackground, 0.9f);
    pressedBackground = darken(baseBackground, 0.75f);
    if (textBox) textBox->setBackgroundColor(baseBackground);
}

void ButtonPrimitive::setTextColor(const simd::float4& color)
{
    if (textBox) textBox->setTextColor(color);
}

void ButtonPrimitive::setPadding(float left, float right, float top, float bottom)
{
    if (textBox) textBox->setPadding(left, right, top, bottom);
}

void ButtonPrimitive::setCallback(std::function<void()> cb)
{
    callback = std::move(cb);
}

void ButtonPrimitive::triggerClick()
{
    if (callback) callback();
}

float ButtonPrimitive::getX() const { return textBox ? textBox->getX() : 0.0f; }
float ButtonPrimitive::getY() const { return textBox ? textBox->getY() : 0.0f; }
float ButtonPrimitive::getWidth() const { return textBox ? textBox->getWidth() : 0.0f; }
float ButtonPrimitive::getHeight() const { return textBox ? textBox->getHeight() : 0.0f; }

void ButtonPrimitive::setPosition(float x, float y)
{
    if (textBox) textBox->setPosition(x, y);
}

void ButtonPrimitive::setSize(float width, float height)
{
    if (textBox) textBox->setSize(width, height);
}

void ButtonPrimitive::setTransform(const simd::float4x4& transform)
{
    if (textBox) textBox->setTransform(transform);
}

void ButtonPrimitive::setHoverVisual(bool hover)
{
    if (textBox) textBox->setBackgroundColor(hover ? hoverBackground : baseBackground);
}

void ButtonPrimitive::setPressedVisual(bool pressed)
{
    if (textBox) textBox->setBackgroundColor(pressed ? pressedBackground : baseBackground);
}
