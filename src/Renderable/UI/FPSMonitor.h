#pragma once

#include "../../lib/View/UIElement.h"
#include <chrono>

class FPSMonitor : public UIElement {
public:
    FPSMonitor(MTL::Device* device, float screenWidth, float screenHeight);
    virtual ~FPSMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;
    // Move the monitor to follow the mouse. Coordinates are in screen/client space
    // with origin at top-left.
    void setPosition(float x, float y);

protected:
    void onSizeChanged() override;

private:
    std::chrono::high_resolution_clock::time_point lastTime;
    int frameCount;
    int fps;
    // position and size of the monitor quad
    float posX = 0.0f;
    float posY = 0.0f;
    float width = 200.0f;
    float height = 100.0f;
};
