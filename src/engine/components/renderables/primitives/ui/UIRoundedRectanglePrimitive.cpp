#include "engine/components/renderables/primitives/ui/UIRoundedRectanglePrimitive.h"
#include "engine/core/LogManager.h"

UIRoundedRectanglePrimitive::UIRoundedRectanglePrimitive(MTL::Device* device,
                                                         float width, float height,
                                                         float radius,
                                                         const simd::float4& color,
                                                         int qualityPerCorner)
    : device(device)
{
    LOG_CONSTRUCT("UIRoundedRectanglePrimitive");
    
    roundedRect = std::make_shared<RoundedRectanglePrimitive>(
        device, 0.0f, 0.0f, width, height, radius, color, qualityPerCorner
    );
    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

void UIRoundedRectanglePrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                        const simd::float4x4& projection,
                                        const simd::float4x4& view)
{
    updatePrimitiveTransform();
    
    if (roundedRect) {
        roundedRect->draw(encoder, projection, view);
    }
}

void UIRoundedRectanglePrimitive::setSize(float width, float height)
{
    transform.setSize(width, height);
    if (roundedRect) {
        roundedRect->setSize(width, height);
    }
}

void UIRoundedRectanglePrimitive::setRadius(float radius)
{
    if (roundedRect) {
        roundedRect->setRadius(radius);
    }
}

void UIRoundedRectanglePrimitive::setQualityPerCorner(int quality)
{
    if (roundedRect) {
        roundedRect->setQualityPerCorner(quality);
    }
}

void UIRoundedRectanglePrimitive::getContentSize(float& width, float& height) const
{
    width = transform.getWidth();
    height = transform.getHeight();
}

void UIRoundedRectanglePrimitive::updatePrimitiveTransform()
{
    simd::float2 pos = transform.getAbsolutePosition();
    roundedRect->setPosition(pos.x, pos.y);
}

void UIRoundedRectanglePrimitive::onColorChanged()
{
    if (roundedRect) {
        roundedRect->setColor(getColor());
    }
}
