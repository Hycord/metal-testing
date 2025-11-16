#include "components/renderables/core/UITransform.h"
#include "systems/input/InputState.h"

UITransform::UITransform()
    : position{0.0f, 0.0f},
      size{0.0f, 0.0f},
      hasAnchor(false),
      parent(nullptr),
      dirty(true)
{
}

void UITransform::setPosition(float x, float y)
{
    if (position.x != x || position.y != y) {
        position = {x, y};
        dirty = true;
    }
}

void UITransform::setSize(float width, float height)
{
    if (size.x != width || size.y != height) {
        size = {width, height};
        dirty = true;
    }
}

void UITransform::setAnchor(const AnchorConfig& config)
{
    anchor = config;
    hasAnchor = true;
    dirty = true;
}

void UITransform::setAnchor(AnchorTarget target, AnchorPoint point, float offsetX, float offsetY)
{
    anchor.target = target;
    anchor.point = point;
    anchor.offsetX = offsetX;
    anchor.offsetY = offsetY;
    hasAnchor = true;
    dirty = true;
}

void UITransform::clearAnchor()
{
    hasAnchor = false;
    dirty = true;
}

void UITransform::setParent(UITransform* p)
{
    if (parent != p) {
        parent = p;
        dirty = true;
    }
}

simd::float2 UITransform::getAbsolutePosition() const
{
    if (!hasAnchor) {
        if (parent) {
            simd::float2 parentPos = parent->getAbsolutePosition();
            return parentPos + position;
        }
        return position;
    }
    
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    
    return calculateAnchorPosition(screenWidth, screenHeight);
}

void UITransform::update(float screenWidth, float screenHeight)
{
    if (hasAnchor) {
        dirty = true;
    }
}

simd::float2 UITransform::calculateAnchorPosition(float screenWidth, float screenHeight) const
{
    simd::float2 anchorBase{0.0f, 0.0f};
    float refWidth = 0.0f;
    float refHeight = 0.0f;
    
    if (anchor.target == AnchorTarget::Screen) {
        refWidth = screenWidth;
        refHeight = screenHeight;
    } else if (anchor.target == AnchorTarget::Parent && parent) {
        simd::float2 parentPos = parent->getAbsolutePosition();
        simd::float2 parentSize = parent->getSize();
        anchorBase = parentPos;
        refWidth = parentSize.x;
        refHeight = parentSize.y;
    } else {
        refWidth = screenWidth;
        refHeight = screenHeight;
    }
    
    simd::float2 anchorOffset = getAnchorPointOffset(anchor.point, refWidth, refHeight);
    simd::float2 elementOffset = getAnchorPointOffset(anchor.point, size.x, size.y);
    
    return anchorBase + anchorOffset - elementOffset + simd::float2{anchor.offsetX, anchor.offsetY};
}

simd::float2 UITransform::getAnchorPointOffset(AnchorPoint point, float width, float height) const
{
    switch (point) {
        case AnchorPoint::TopLeft:
            return {0.0f, height};
        case AnchorPoint::TopCenter:
            return {width * 0.5f, height};
        case AnchorPoint::TopRight:
            return {width, height};
        case AnchorPoint::CenterLeft:
            return {0.0f, height * 0.5f};
        case AnchorPoint::Center:
            return {width * 0.5f, height * 0.5f};
        case AnchorPoint::CenterRight:
            return {width, height * 0.5f};
        case AnchorPoint::BottomLeft:
            return {0.0f, 0.0f};
        case AnchorPoint::BottomCenter:
            return {width * 0.5f, 0.0f};
        case AnchorPoint::BottomRight:
            return {width, 0.0f};
    }
    return {0.0f, 0.0f};
}
