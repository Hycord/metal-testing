#include "engine/components/renderables/core/WorldElement.h"
#include "engine/core/LogManager.h"

WorldElement::WorldElement(MTL::Device* device)
    : device(device)
{
    LOG_CONSTRUCT("WorldElement");
}

WorldElement::~WorldElement()
{
    LOG_DESTROY("WorldElement");
}

void WorldElement::drawPrimitives(MTL::RenderCommandEncoder* encoder,
                                  const simd::float4x4& projection,
                                  const simd::float4x4& view)
{
    for (auto& prim : primitives) {
        if (prim) {
            prim->draw(encoder, projection, view);
        }
    }
}

void WorldElement::addPrimitive(const std::shared_ptr<RenderablePrimitive>& prim)
{
    if (prim) {
        primitives.push_back(prim);
        LOG_INFO("WorldElement: added primitive (total: {})", static_cast<int>(primitives.size()));
    }
}

void WorldElement::clearPrimitives()
{
    primitives.clear();
    LOG_INFO("WorldElement: cleared all primitives");
}

void WorldElement::setPosition(float x, float y, float z)
{
    position = {x, y, z};
}

void WorldElement::setRotation(float pitch, float yaw, float roll)
{
    rotation = {pitch, yaw, roll};
}

void WorldElement::getContentSize(float& width, float& height) const
{
    width = 0.0f;
    height = 0.0f;
    
    for (const auto& prim : primitives) {
        if (prim) {
            float pw, ph;
            prim->getContentSize(pw, ph);
            width = std::max(width, pw);
            height = std::max(height, ph);
        }
    }
}

void WorldElement::render(MTL::RenderCommandEncoder* encoder,
                          const simd::float4x4& projection,
                          const simd::float4x4& view)
{
    drawPrimitives(encoder, projection, view);
}
