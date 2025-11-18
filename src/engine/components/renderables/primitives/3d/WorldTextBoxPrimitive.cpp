#include "engine/components/renderables/primitives/3d/WorldTextBoxPrimitive.h"
#include "engine/core/LogManager.h"
#include "engine/utils/Math.h"

WorldTextBoxPrimitive::WorldTextBoxPrimitive(MTL::Device* device,
                                             const std::string& text,
                                             float x, float y, float z,
                                             float worldWidth, float worldHeight,
                                             const std::string& fontPath,
                                             const TextBoxConfig& config)
    : device(device),
      position{x, y, z},
      forward{0.0f, 0.0f, 1.0f},
      up{0.0f, 1.0f, 0.0f},
      worldWidth(worldWidth),
      worldHeight(worldHeight),
      calculatedFontSize(32.0f)
{
    LOG_CONSTRUCT("WorldTextBoxPrimitive");
    
    setScreenSpace(false);
    
    float pixelWidth = 512.0f;
    float pixelHeight = 512.0f;
    
    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        text,
        0.0f, 0.0f,
        pixelWidth, pixelHeight,
        fontPath,
        32.0f,
        config
    );
    
    textBox->setScreenSpace(false);
    
    updateTransform();
}

void WorldTextBoxPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                  const simd::float4x4& projection,
                                  const simd::float4x4& view)
{
    if (textBox) {
        textBox->draw(encoder, projection, view);
    }
}

void WorldTextBoxPrimitive::setText(const std::string& text)
{
    if (textBox) {
        textBox->setText(text);
    }
}

void WorldTextBoxPrimitive::setTextAlignment(TextAlign align)
{
    if (textBox) {
        textBox->setTextAlignment(align);
    }
}

void WorldTextBoxPrimitive::setTextJustification(TextJustify justify)
{
    if (textBox) {
        textBox->setTextJustification(justify);
    }
}

void WorldTextBoxPrimitive::setBackgroundColor(const simd::float4& color)
{
    if (textBox) {
        textBox->setBackgroundColor(color);
    }
}

void WorldTextBoxPrimitive::setTextColor(const simd::float4& color)
{
    if (textBox) {
        textBox->setTextColor(color);
    }
}

void WorldTextBoxPrimitive::setPadding(float left, float right, float top, float bottom)
{
    if (textBox) {
        textBox->setPadding(left, right, top, bottom);
    }
}

void WorldTextBoxPrimitive::setPosition(float x, float y, float z)
{
    position = {x, y, z};
    updateTransform();
}

void WorldTextBoxPrimitive::setWorldSize(float width, float height)
{
    worldWidth = width;
    worldHeight = height;
    updateTransform();
}

void WorldTextBoxPrimitive::setOrientation(const simd::float3& newForward, const simd::float3& newUp)
{
    forward = simd::normalize(newForward);
    up = simd::normalize(newUp);
    updateTransform();
}

const std::string& WorldTextBoxPrimitive::getText() const
{
    static std::string empty;
    return textBox ? textBox->getText() : empty;
}

void WorldTextBoxPrimitive::getContentSize(float& width, float& height) const
{
    width = worldWidth;
    height = worldHeight;
}

void WorldTextBoxPrimitive::updateTransform()
{
    simd::float3 right = simd::normalize(simd::cross(up, forward));
    simd::float3 actualUp = simd::cross(forward, right);
    
    simd::float4x4 rotationMatrix;
    rotationMatrix.columns[0] = simd::make_float4(right.x, right.y, right.z, 0.0f);
    rotationMatrix.columns[1] = simd::make_float4(actualUp.x, actualUp.y, actualUp.z, 0.0f);
    rotationMatrix.columns[2] = simd::make_float4(forward.x, forward.y, forward.z, 0.0f);
    rotationMatrix.columns[3] = simd::make_float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float pixelSize = 512.0f;
    float scaleX = worldWidth / pixelSize;
    float scaleY = worldHeight / pixelSize;
    simd::float4x4 scaleMatrix = MetalMath::scale(scaleX, scaleY, 1.0f);
    
    simd::float4x4 translationMatrix = MetalMath::translate(position.x, position.y, position.z);
    
    simd::float4x4 transform = translationMatrix * rotationMatrix * scaleMatrix;
    
    if (textBox) {
        textBox->setTransform(transform);
    }
    
    LOG_DEBUG("WorldTextBoxPrimitive: updated transform at ({}, {}, {}) size ({}, {})",
              position.x, position.y, position.z, worldWidth, worldHeight);
}

void WorldTextBoxPrimitive::onColorChanged()
{
}

void WorldTextBoxPrimitive::onTransformChanged()
{
}
