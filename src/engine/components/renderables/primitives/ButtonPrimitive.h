#pragma once

#include "engine/components/renderables/primitives/2d/TextBoxPrimitive.h"
#include <memory>
#include <functional>

class ButtonPrimitive {
public:
    ButtonPrimitive(MTL::Device* device,
                    const std::string& text,
                    const std::string& fontPath,
                    float fontSize,
                    const TextBoxConfig& config = TextBoxConfig{});

    ButtonPrimitive(MTL::Device* device,
                    const std::string& text,
                    float x, float y,
                    float width, float height,
                    const std::string& fontPath,
                    float fontSize,
                    const TextBoxConfig& config = TextBoxConfig{});

    ~ButtonPrimitive();

    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view);

    void setText(const std::string& text);
    const std::string& getText() const;

    void setBackgroundColor(const simd::float4& color);
    void setTextColor(const simd::float4& color);

    void setPadding(float left, float right, float top, float bottom);

    void setCallback(std::function<void()> cb);
    void triggerClick();

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setTransform(const simd::float4x4& transform);

    void setHoverVisual(bool hover);
    void setPressedVisual(bool pressed);

    std::shared_ptr<TextBoxPrimitive> underlying() const { return textBox; }

private:
    MTL::Device* device;
    std::shared_ptr<TextBoxPrimitive> textBox;
    std::function<void()> callback;
    simd::float4 baseBackground;
    simd::float4 hoverBackground;
    simd::float4 pressedBackground;
};
