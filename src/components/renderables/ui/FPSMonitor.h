#pragma once

#include "components/renderables/core/UIElement.h"
#include "components/renderables/primitives/UIPrimitiveRoundedRectangle.h"
#include <chrono>

class FPSMonitor : public UIElement {
public:
    FPSMonitor(MTL::Device* device);
    virtual ~FPSMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;

private:
    // position and size of the monitor quad
    float posX;
    float posY;
    float width;
    float height;

    // Example primitive composed within this UIElement
    std::shared_ptr<RoundedRectangleUIPrimitive> demoRectangle;
};
