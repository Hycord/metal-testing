#pragma once

#include "components/renderables/core/UIElement.h"
#include "components/renderables/primitives/RenderablePrimitive.h"
#include "components/renderables/primitives/2d/TextBoxPrimitive.h"
#include <chrono>

class DebugMonitor : public UIElement {
public:
    DebugMonitor(MTL::Device* device);
    virtual ~DebugMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;

private:
    
    float posX;
    float posY;

    
    std::shared_ptr<TextBoxPrimitive> textBox;

    
    std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();
    double accumSeconds = 0.0;
    int framesAccum = 0;
    double smoothedFps = 0.0;
    
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
};
