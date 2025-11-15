#pragma once

#include "components/renderables/core/UIElement.h"
#include "components/renderables/primitives/UIPrimitive.h"
#include <chrono>

class TextPrimitive;

class FPSMonitor : public UIElement {
public:
    FPSMonitor(MTL::Device* device);
    virtual ~FPSMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;

private:
    
    float posX;
    float posY;
    float width;
    float height;

    
    std::shared_ptr<RoundedRectangleUIPrimitive> demoRectangle;
    
    
    std::shared_ptr<TextPrimitive> helloText;

    
    std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();
    double accumSeconds = 0.0;
    int framesAccum = 0;
    double smoothedFps = 0.0;

    
    std::chrono::steady_clock::time_point lastDebugLog = std::chrono::steady_clock::now();
};
