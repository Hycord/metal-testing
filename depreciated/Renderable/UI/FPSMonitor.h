#pragma once

#include "../../lib/View/UIElement.h"
#include "../../lib/View/UIPrimitives.h"
#include <chrono>

class FPSMonitor : public UIElement {
public:
    FPSMonitor(MTL::Device* device);
    virtual ~FPSMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;

private:
    std::chrono::high_resolution_clock::time_point lastTime;
    int frameCount;
    int fps;
    // position and size of the monitor quad
    float posX = 0.0f;
    float posY = 0.0f;
    float width = 200.0f;
    float height = 100.0f;

    // Example primitive composed within this UIElement
    std::shared_ptr<RectangleUIPrimitive> demoRect;
};
