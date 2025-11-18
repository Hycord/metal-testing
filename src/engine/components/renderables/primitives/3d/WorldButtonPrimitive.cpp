#include "engine/components/renderables/primitives/3d/WorldButtonPrimitive.h"
#include "engine/core/LogManager.h"
#include "engine/systems/input/InputState.h"
#include "engine/utils/Math.h"
#include <algorithm>

WorldButtonPrimitive::WorldButtonPrimitive(MTL::Device* device,
                                           const std::string& text,
                                           float x, float y, float z,
                                           float worldWidth, float worldHeight,
                                           const std::string& fontPath,
                                           const TextBoxConfig& config)
    : device(device), position{x,y,z}, worldWidth(worldWidth), worldHeight(worldHeight)
{
    LOG_CONSTRUCT("WorldButtonPrimitive");

    button = std::make_shared<ButtonPrimitive>(device, text, 0.0f, 0.0f, worldWidth, worldHeight, fontPath, 32.0f, config);

    forward = simd::float3{0,0,1};
    up = simd::float3{0,1,0};
}

WorldButtonPrimitive::~WorldButtonPrimitive()
{
    LOG_DESTROY("WorldButtonPrimitive");
}

void WorldButtonPrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                const simd::float4x4& projection,
                                const simd::float4x4& view)
{
    updateTransform();

    const auto& mouse = InputState::getMouseState();
    bool leftDown = mouse.leftButton;
    float mx = mouse.x;
    float my = mouse.y;

    bool inside = pointInProjectedRect(mx, my, projection, view);

    if (inside && !isHover) {
        isHover = true;
        button->setHoverVisual(true);
    } else if (!inside && isHover) {
        isHover = false;
        button->setHoverVisual(false);
    }

    if (leftDown && inside) {
        if (!isPressed) {
            isPressed = true;
            button->setPressedVisual(true);
        }
    } else {
        if (isPressed) {
            if (inside) button->triggerClick();
            isPressed = false;
            button->setPressedVisual(false);
        }
    }

    if (button) button->draw(encoder, projection, view);
}

void WorldButtonPrimitive::setText(const std::string& text)
{
    if (button) button->setText(text);
}

void WorldButtonPrimitive::setCallback(std::function<void()> cb)
{
    if (button) button->setCallback(cb);
}

void WorldButtonPrimitive::setPosition(float x, float y, float z)
{
    position = simd::float3{x,y,z};
}

void WorldButtonPrimitive::setWorldSize(float width, float height)
{
    worldWidth = width;
    worldHeight = height;
}

void WorldButtonPrimitive::setOrientation(const simd::float3& f, const simd::float3& u)
{
    forward = f;
    up = u;
}

void WorldButtonPrimitive::getContentSize(float& width, float& height) const
{
    width = worldWidth;
    height = worldHeight;
}

simd::float3 WorldButtonPrimitive::getPosition() const { return position; }

void WorldButtonPrimitive::updateTransform()
{
    simd::float3 right = simd::normalize(simd::cross(forward, up));
    simd::float3 u = simd::normalize(up);

    simd::float3 halfW = right * (worldWidth * 0.5f);
    simd::float3 halfH = u * (worldHeight * 0.5f);

    simd::float3 tl = position - halfW + halfH;
    simd::float3 tr = position + halfW + halfH;
    simd::float3 bl = position - halfW - halfH;
    simd::float3 br = position + halfW - halfH;

    // For rendering, set a transform that maps to world position
    // Build model matrix from right, up, forward and position
    simd::float4x4 model = simd::float4x4{
        simd::float4{right.x, right.y, right.z, 0.0f},
        simd::float4{u.x, u.y, u.z, 0.0f},
        simd::float4{forward.x, forward.y, forward.z, 0.0f},
        simd::float4{position.x, position.y, position.z, 1.0f}
    };

    button->setTransform(model);
}

bool WorldButtonPrimitive::pointInProjectedRect(float mx, float my,
                                                const simd::float4x4& projection,
                                                const simd::float4x4& view) const
{
    simd::float3 right = simd::normalize(simd::cross(forward, up));
    simd::float3 u = simd::normalize(up);

    simd::float3 halfW = right * (worldWidth * 0.5f);
    simd::float3 halfH = u * (worldHeight * 0.5f);

    simd::float3 corners[4];
    corners[0] = position - halfW + halfH; // tl
    corners[1] = position + halfW + halfH; // tr
    corners[2] = position - halfW - halfH; // bl
    corners[3] = position + halfW - halfH; // br

    float sx[4], sy[4];
    int w = static_cast<int>(InputState::getWindowWidth());
    int h = static_cast<int>(InputState::getWindowHeight());

    for (int i = 0; i < 4; ++i) {
        simd::float4 clip = projection * view * simd::float4{corners[i].x, corners[i].y, corners[i].z, 1.0f};
        if (clip.w == 0.0f) return false;
        simd::float3 ndc = simd::float3{clip.x / clip.w, clip.y / clip.w, clip.z / clip.w};
        sx[i] = (ndc.x * 0.5f + 0.5f) * w;
        sy[i] = ( -ndc.y * 0.5f + 0.5f) * h;
    }

    float minX = *std::min_element(sx, sx+4);
    float maxX = *std::max_element(sx, sx+4);
    float minY = *std::min_element(sy, sy+4);
    float maxY = *std::max_element(sy, sy+4);

    return (mx >= minX && mx <= maxX && my >= minY && my <= maxY);
}
