#include "engine/components/renderables/primitives/ui/UIRectanglePrimitive.h"
#include "engine/core/LogManager.h"

UIRectanglePrimitive::UIRectanglePrimitive(MTL::Device* device, float width, float height, const simd::float4& color)
    : device(device)
{
    LOG_CONSTRUCT("UIRectanglePrimitive");
    
    rectangle = std::make_shared<RectanglePrimitive>(device, 0.0f, 0.0f, width, height, color);
    transform.setSize(width, height);
    transform.setPosition(0.0f, 0.0f);
}

void UIRectanglePrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                 const simd::float4x4& projection,
                                 const simd::float4x4& view)
{
    updatePrimitiveTransform();
    
    if (rectangle) {
        rectangle->draw(encoder, projection, view);
    }
}

void UIRectanglePrimitive::setSize(float width, float height)
{
    transform.setSize(width, height);
    if (rectangle) {
        rectangle->setSize(width, height);
    }
}

void UIRectanglePrimitive::getContentSize(float& width, float& height) const
{
    width = transform.getWidth();
    height = transform.getHeight();
}

void UIRectanglePrimitive::updatePrimitiveTransform()
{
    simd::float2 pos = transform.getAbsolutePosition();
    rectangle->setPosition(pos.x, pos.y);
}

void UIRectanglePrimitive::onColorChanged()
{
    if (rectangle) {
        rectangle->setColor(getColor());
    }
}
