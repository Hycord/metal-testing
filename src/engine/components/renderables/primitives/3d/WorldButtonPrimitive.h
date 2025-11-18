#pragma once

#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include "engine/components/renderables/primitives/ButtonPrimitive.h"
#include <memory>
#include <string>

class WorldButtonPrimitive : public RenderablePrimitive {
public:
    WorldButtonPrimitive(MTL::Device* device,
                         const std::string& text,
                         float x, float y, float z,
                         float worldWidth, float worldHeight,
                         const std::string& fontPath,
                         const TextBoxConfig& config = TextBoxConfig{});

    ~WorldButtonPrimitive();

    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4& projection,
              const simd::float4x4& view) override;

    void setText(const std::string& text);
    void setCallback(std::function<void()> cb);

    void setPosition(float x, float y, float z);
    void setWorldSize(float width, float height);
    void setOrientation(const simd::float3& forward, const simd::float3& up);

    void getContentSize(float& width, float& height) const override;

    simd::float3 getPosition() const;

private:
    void updateTransform();
    bool pointInProjectedRect(float mx, float my,
                              const simd::float4x4& projection,
                              const simd::float4x4& view) const;
    void onColorChanged() override {}
    void onTransformChanged() override {}

    MTL::Device* device;
    std::shared_ptr<ButtonPrimitive> button;
    simd::float3 position;
    simd::float3 forward;
    simd::float3 up;
    float worldWidth;
    float worldHeight;

    bool isHover = false;
    bool isPressed = false;
};
