#include "components/renderables/ui/FPSMonitor.h"
#include "components/renderables/primitives/2d/TextPrimitive.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include "core/LogManager.h"
#include "systems/input/InputState.h"
#include "utils/Path.h"
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
    
    const float marginX = 10.0f;
    const float marginY = 10.0f;
    posX = windowWidth - width - marginX;
    posY = marginY;

    
    
    float radius = 12.0f;
    int segments = 32;
    demoRectangle = std::make_shared<RoundedRectangleUIPrimitive>(
        device,
        posX, posY,
        width, height,
        radius,
        
        simd::float4{0.50f, 0.50f, 0.50f, 1.0f},
        segments
    );
    addPrimitive(demoRectangle);
    
    
    std::string fontPath = Path::dataPath("fonts/Roboto/Roboto-Medium.ttf");
    float fontSize = 32.0f;
    
    helloText = std::make_shared<TextPrimitive>(
        device,
        "hello",
        posX,
        posY,
        fontPath,
        fontSize,
        simd::float4{1.0f, 1.0f, 1.0f, 1.0f} 
    );
    
    
    helloText->setBoxSize(width, height);
    helloText->setAlignment(TextAlign::Center);
    helloText->setJustification(TextJustify::End); 
    
    addPrimitive(helloText);

    enableAutoAnchor(AnchorCorner::BottomRight, 10.0f, 10.0f);
    
}

FPSMonitor::~FPSMonitor()
{
}

void FPSMonitor::render(MTL::RenderCommandEncoder *encoder)
{
    
    auto now = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(now - lastTick).count();
    lastTick = now;
    accumSeconds += dt;
    framesAccum += 1;
    if (accumSeconds >= 0.25) {
        double instFps = framesAccum / accumSeconds;
        
        if (smoothedFps <= 0.0) smoothedFps = instFps;
        else smoothedFps = smoothedFps * 0.8 + instFps * 0.2;
        
        framesAccum = 0;
        accumSeconds = 0.0;
    }

    
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
    
    
    if (helloText) {
        helloText->setPosition(left, top);
        helloText->setBoxSize(width, height);
    }
    
    
    drawPrimitives(encoder);
}
