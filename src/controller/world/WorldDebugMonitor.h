#pragma once

#include "engine/components/renderables/core/WorldElement.h"
#include "engine/components/renderables/primitives/3d/WorldTextBoxPrimitive.h"
#include "engine/components/renderables/DebugData.h"
#include <memory>
#include <string>
#include <chrono>

class WorldDebugMonitor : public WorldElement {
public:
    WorldDebugMonitor(MTL::Device* device, 
                     float x, float y, float z,
                     float pitch = 0.0f,
                     float yaw = 0.0f,
                     float roll = 0.0f);
    virtual ~WorldDebugMonitor();
    
    void render(MTL::RenderCommandEncoder* encoder,
                const simd::float4x4& projection,
                const simd::float4x4& view) override;
    
    void setMessage(const std::string& message);

private:
    static DebugData createSizingDefaults();
    DebugData buildDebugData() const;
    std::string formatDebugText(const DebugData& data) const;
    
    std::shared_ptr<WorldTextBoxPrimitive> textBox;
    
    std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    double accumSeconds = 0.0;
    double smoothedFps = 0.0;
    int framesAccum = 0;
};
