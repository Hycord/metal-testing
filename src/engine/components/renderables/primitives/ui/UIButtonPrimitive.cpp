#include "engine/components/renderables/primitives/ui/UIButtonPrimitive.h"
#include "engine/core/LogManager.h"
#include "engine/systems/input/InputState.h"

UIButtonPrimitive::~UIButtonPrimitive()
{
    LOG_DESTROY("UIButtonPrimitive");
}

UIButtonPrimitive::UIButtonPrimitive(MTL::Device* device,
                                     const std::string& text,
                                     const std::string& fontPath,
                                     float fontSize,
                                     const TextBoxConfig& config)
    : device(device)
{
    LOG_CONSTRUCT("UIButtonPrimitive");

    button = std::make_shared<ButtonPrimitive>(device, text, fontPath, fontSize, config);

    float w, h;
    button->underlying()->getContentSize(w, h);
    transform.setSize(w, h);
    transform.setPosition(0.0f, 0.0f);
}

UIButtonPrimitive::UIButtonPrimitive(MTL::Device* device,
                                     const std::string& text,
                                     float width, float height,
                                     const std::string& fontPath,
                                     float fontSize,
                                     const TextBoxConfig& config)
    : device(device)
{
    LOG_CONSTRUCT("UIButtonPrimitive");

    button = std::make_shared<ButtonPrimitive>(device, text, 0.0f, 0.0f, width, height, fontPath, fontSize, config);

    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

void UIButtonPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                             const simd::float4x4& projection,
                             const simd::float4x4& view)
{
    updatePrimitivePosition();

    const auto& mouse = InputState::getMouseState();
    bool leftDown = mouse.leftButton;
    float mx = mouse.x;
    float my = mouse.y;
    float screenH = InputState::getWindowHeight();
    float myBottomOrigin = screenH - my;

    float x = transform.getAbsolutePosition().x;
    float y = transform.getAbsolutePosition().y;
    float w = transform.getWidth();
    float h = transform.getHeight();

    bool inside = (mx >= x && mx <= x + w && myBottomOrigin >= y && myBottomOrigin <= y + h);

    LOG_DEBUG("Button x={} y={} w={} h={}, mouse x={} y(top)={}, y(bottom)={}, inside={}, leftDown={}", 
              x, y, w, h, mx, my, myBottomOrigin, inside, leftDown);

    if (inside && !isHover) {
        isHover = true;
        button->setHoverVisual(true);
        LOG_DEBUG("Button hover ON");
    } else if (!inside && isHover) {
        isHover = false;
        button->setHoverVisual(false);
        LOG_DEBUG("Button hover OFF");
    }

    if (leftDown && inside) {
        if (!isPressed) {
            isPressed = true;
            button->setPressedVisual(true);
            LOG_DEBUG("Button pressed");
        }
    } else {
        if (isPressed) {
            LOG_DEBUG("Button released, inside: {}", inside);
            if (inside) button->triggerClick();
            isPressed = false;
            button->setPressedVisual(false);
        }
    }

    if (button) button->draw(encoder, projection, view);
}

void UIButtonPrimitive::setText(const std::string& text)
{
    if (button) {
        button->setText(text);
        float w, h;
        button->underlying()->getContentSize(w, h);
        transform.setSize(w, h);
    }
}

void UIButtonPrimitive::setCallback(std::function<void()> cb)
{
    if (button) button->setCallback(cb);
}

void UIButtonPrimitive::getContentSize(float& width, float& height) const
{
    width = transform.getWidth();
    height = transform.getHeight();
}

void UIButtonPrimitive::updatePrimitivePosition()
{
    simd::float2 pos = transform.getAbsolutePosition();
    button->setPosition(pos.x, pos.y);
}
