#pragma once

#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include "engine/components/renderables/core/UITransform.h"
#include "engine/components/renderables/primitives/ButtonPrimitive.h"
#include <memory>

class UIButtonPrimitive : public RenderablePrimitive {
public:
    UIButtonPrimitive(MTL::Device* device,
                      const std::string& text,
                      const std::string& fontPath,
                      float fontSize,
                      const TextBoxConfig& config = TextBoxConfig{});

    UIButtonPrimitive(MTL::Device* device,
                      const std::string& text,
                      float width, float height,
                      const std::string& fontPath,
                      float fontSize,
                      const TextBoxConfig& config = TextBoxConfig{});

    ~UIButtonPrimitive();

    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;

    void setText(const std::string& text);
    void setCallback(std::function<void()> cb);

    UITransform& getTransform() { return transform; }
    const UITransform& getTransformConst() const { return transform; }

    void getContentSize(float& width, float& height) const override;

private:
    void updatePrimitivePosition();
    void onColorChanged() override {}

    MTL::Device* device;
    UITransform transform;
    std::shared_ptr<ButtonPrimitive> button;

    bool isPressed = false;
    bool isHover = false;
};
