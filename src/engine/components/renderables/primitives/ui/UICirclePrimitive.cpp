#include "engine/components/renderables/primitives/ui/UICirclePrimitive.h"
#include "engine/core/LogManager.h"

UICirclePrimitive::UICirclePrimitive(MTL::Device* device, float radius, const simd::float4& color, int segments)
    : device(device),
      radius(radius)
{
    LOG_CONSTRUCT("UICirclePrimitive");
    
    circle = std::make_shared<CirclePrimitive>(device, 0.0f, 0.0f, radius, color, segments);
    transform.setSize(radius * 2.0f, radius * 2.0f);
    transform.setPosition(0.0f, 0.0f);
}

void UICirclePrimitive::draw(MTL::RenderCommandEncoder* encoder,
                              const simd::float4x4& projection,
                              const simd::float4x4& view)
{
    updatePrimitiveTransform();
    
    if (circle) {
        circle->draw(encoder, projection, view);
    }
}

void UICirclePrimitive::setRadius(float r)
{
    radius = r;
    transform.setSize(radius * 2.0f, radius * 2.0f);
    if (circle) {
        circle->setRadius(r);
    }
}

void UICirclePrimitive::setSegments(int segments)
{
}

void UICirclePrimitive::getContentSize(float& width, float& height) const
{
    width = radius * 2.0f;
    height = radius * 2.0f;
}

void UICirclePrimitive::updatePrimitiveTransform()
{
    simd::float2 pos = transform.getAbsolutePosition();
    circle->setCenter(pos.x + radius, pos.y + radius);
}

void UICirclePrimitive::onColorChanged()
{
    if (circle) {
        circle->setColor(getColor());
    }
}
