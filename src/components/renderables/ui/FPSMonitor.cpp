#include "components/renderables/ui/FPSMonitor.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include "systems/input/InputState.h"
#include <iostream>
#include <chrono>
#include <cmath>

FPSMonitor::FPSMonitor(MTL::Device *device)
    : UIElement(device),
      width(200.0f),
            height(100.0f)
{
    const float windowWidth = InputState::getWindowWidth();
    const float windowHeight = InputState::getWindowHeight();
    // Place at bottom-right with 10px margins; origin is bottom-left
    const float marginX = 10.0f;
    const float marginY = 10.0f;
    posX = windowWidth - width - marginX;
    posY = marginY;

    // Build a rounded rectangle background matching the quad corners
    // Use a modest radius; tweak per visual preference
    float radius = 6.0f;
    demoRectangle = std::make_shared<RoundedRectangleUIPrimitive>(
        device,
        posX, posY,
        width, height,
        radius,
        simd::float4{1.0f, 1.0f, 1.0f, 1.0f},
        4 // segments per corner
    );
    addPrimitive(demoRectangle);
    enableAutoAnchor(AnchorCorner::BottomRight, 10.0f, 10.0f);
    
}

FPSMonitor::~FPSMonitor()
{
}

void FPSMonitor::render(MTL::RenderCommandEncoder *encoder)
{
    // Keep the rounded rectangle anchored to bottom-right with the same margins
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    const float marginX = 10.0f;
    const float marginY = 10.0f;
    float left = screenWidth - width - marginX;
    float top = marginY;
    if (demoRectangle) {
        demoRectangle->setPosition(left, top);
        demoRectangle->setSize(width, height);
    }
    // Draw only primitives (rounded rectangle background)
    drawPrimitives(encoder);
}
