#include "components/renderables/ui/FPSMonitor.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include "systems/input/InputState.h"
#include <iostream>
#include <chrono>
#include <cmath>

FPSMonitor::FPSMonitor(MTL::Device *device)
    : UIElement(device), lastTime(std::chrono::high_resolution_clock::now()), frameCount(0), fps(0)
{
    const float windowWidth = InputState::getWindowWidth();
    const float windowHeight = InputState::getWindowHeight();
    posX = windowWidth - width;
    posY = windowHeight - height;
    buildCachedQuad(posX, posY, width, height, {0.0f, 1.0f, 0.0f, 1.0f});

    // Demo: add a 50x50 red rectangle at the top-left corner of this UI element
    demoRect = std::make_shared<RectangleUIPrimitive>(device, posX, posY, 50.0f, 50.0f, simd::float4{1.0f, 0.0f, 0.0f, 1.0f});
    demoRect->setPrimitiveType(MTL::PrimitiveType::PrimitiveTypeTriangle);
    addPrimitive(demoRect);
}

FPSMonitor::~FPSMonitor()
{
}

void FPSMonitor::render(MTL::RenderCommandEncoder *encoder)
{
    // Update position from InputState (follow the OS cursor with offset)
    const float windowWidth = InputState::getWindowWidth();
    const float windowHeight = InputState::getWindowHeight();
    const float mouseX = static_cast<float>(InputState::getMouseX()) + 12.0f;
    const float mouseY = static_cast<float>(InputState::getMouseY()) + 12.0f;

    const float desiredX = mouseX;
    const float desiredY = windowHeight - mouseY - height;

    // Update position only if moved enough to avoid rebuilding every frame
    posX = desiredX;
    posY = desiredY;
    // Move the existing cached quad without recreating shader/material
    moveCachedQuad(posX, posY, width, height);
    if (demoRect)
    {
        // Keep demo rectangle pinned to the top-left of the monitor
        demoRect->setPosition(posX, posY);
    }

    drawCachedQuad(encoder);
}
