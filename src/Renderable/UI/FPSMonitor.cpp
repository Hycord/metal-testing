#include "FPSMonitor.h"
#include "../../lib/View/MeshFactory.h"
#include "../../lib/View/Shader.h"
#include <iostream>
#include <chrono>

FPSMonitor::FPSMonitor(MTL::Device* device, float screenWidth, float screenHeight)
    : UIElement(device, screenWidth, screenHeight), lastTime(std::chrono::high_resolution_clock::now()), frameCount(0), fps(0)
{
    // Build initial cached quad at the default position (bottom-right)
    onSizeChanged();
}

FPSMonitor::~FPSMonitor()
{
}

void FPSMonitor::onSizeChanged()
{
    // Position the quad at posX/posY if set; otherwise default to bottom-right
    float left = (posX != 0.0f || posY != 0.0f) ? posX : (screenWidth - width);
    float top = (posX != 0.0f || posY != 0.0f) ? posY : (screenHeight - height);
    buildCachedQuad(left, top, width, height, {0.0f, 1.0f, 0.0f, 1.0f});
}

void FPSMonitor::render(MTL::RenderCommandEncoder *encoder)
{
    drawCachedQuad(encoder);

}

void FPSMonitor::setPosition(float x, float y)
{
    // Update position only if moved enough to avoid rebuilding every frame
    if (std::abs(posX - x) < 1.0f && std::abs(posY - y) < 1.0f)
        return;
    posX = x;
    posY = y;
    // Move the existing cached quad without recreating shader/material
    moveCachedQuad(posX, posY, width, height);
}