#pragma once

#include <simd/simd.h>
#include <memory>

enum class AnchorPoint {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

enum class AnchorTarget {
    Screen,
    Parent
};

struct AnchorConfig {
    AnchorTarget target = AnchorTarget::Screen;
    AnchorPoint point = AnchorPoint::TopLeft;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
};

class UITransform {
public:
    UITransform();
    ~UITransform() = default;
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    
    void setAnchor(const AnchorConfig& config);
    void setAnchor(AnchorTarget target, AnchorPoint point, float offsetX = 0.0f, float offsetY = 0.0f);
    void clearAnchor();
    
    void setParent(UITransform* parent);
    UITransform* getParent() const { return parent; }
    
    simd::float2 getAbsolutePosition() const;
    simd::float2 getLocalPosition() const { return position; }
    simd::float2 getSize() const { return size; }
    
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getWidth() const { return size.x; }
    float getHeight() const { return size.y; }
    
    void update(float screenWidth, float screenHeight);
    
    bool isDirty() const { return dirty; }
    void clearDirty() { dirty = false; }

private:
    simd::float2 position;
    simd::float2 size;
    
    bool hasAnchor;
    AnchorConfig anchor;
    
    UITransform* parent;
    
    mutable bool dirty;
    
    simd::float2 calculateAnchorPosition(float screenWidth, float screenHeight) const;
    simd::float2 getAnchorPointOffset(AnchorPoint point, float width, float height) const;
};
