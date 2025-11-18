#pragma once

#include "engine/components/renderables/core/UIElement.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include "engine/components/renderables/primitives/ui/UITextBoxPrimitive.h"
#include "engine/components/renderables/DebugData.h"
#include <chrono>
#include <string>

class DebugMonitor : public UIElement {
public:
    DebugMonitor(MTL::Device* device);
    virtual ~DebugMonitor();
    void render(MTL::RenderCommandEncoder *encoder) override;

private:
    static DebugData createSizingDefaults();
    DebugData buildDebugData() const;
    std::string formatDebugText(const DebugData& data) const;
    
    std::shared_ptr<UITextBoxPrimitive> textBox;

    std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    double accumSeconds = 0.0;
    double smoothedFps = 0.0;
    int framesAccum = 0;
};
