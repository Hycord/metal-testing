#include "components/renderables/ui/DebugMonitor.h"
#include "components/renderables/primitives/2d/TextBoxPrimitive.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include "core/LogManager.h"
#include "systems/input/InputState.h"
#include "utils/Path.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>

DebugMonitor::DebugMonitor(MTL::Device *device)
    : UIElement(device)
{
    const float windowWidth = InputState::getWindowWidth();
    const float windowHeight = InputState::getWindowHeight();
    
    const float marginX = 10.0f;
    const float marginY = 10.0f;
    posX = marginX;
    posY = marginY;

    std::string fontPath = Path::dataPath("fonts/Roboto/Roboto-Light.ttf");
    float fontSize = 16.0f;
    
    TextBoxConfig config;
    config.paddingLeft = 15.0f;
    config.paddingRight = 15.0f;
    config.paddingTop = 15.0f;
    config.paddingBottom = 15.0f;
    config.cornerRadius = 12.0f;
    config.cornerSegments = 32;
    config.backgroundColor = simd::float4{0.10f, 0.10f, 0.10f, 0.85f};
    config.textColor = simd::float4{0.9f, 0.9f, 0.9f, 1.0f};
    config.enableWordWrap = true;
    
    textBox = std::make_shared<TextBoxPrimitive>(
        device,
        "Initializing...",
        posX, posY,
        300.0f, 200.0f,
        fontPath,
        fontSize,
        config
    );
    
    textBox->getTransform().setParent(&getTransform());
    textBox->getTransform().setAnchor(AnchorTarget::Parent, AnchorPoint::TopLeft, 0.0f, 0.0f);
    
    addPrimitive(textBox);

    getTransform().setSize(300.0f, 200.0f);
    enableAutoAnchor(AnchorCorner::TopRight, 10.0f, 10.0f);
}

DebugMonitor::~DebugMonitor()
{
}

void DebugMonitor::render(MTL::RenderCommandEncoder *encoder)
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
        
        
        const auto& mouseState = InputState::getMouseState();
        const float screenWidth = InputState::getWindowWidth();
        const float screenHeight = InputState::getWindowHeight();
        
        
        double uptime = std::chrono::duration<double>(now - startTime).count();
        int uptimeSeconds = static_cast<int>(uptime) % 60;
        int uptimeMinutes = static_cast<int>(uptime) / 60;
        
        
        int activeKeys = 0;
        const auto& keyState = InputState::getKeyboardState();
        for (int i = 0; i < GLFW_KEY_LAST; ++i) {
            if (keyState.keys[i]) activeKeys++;
        }
        
        
        int mouseButtons = 0;
        if (mouseState.leftButton) mouseButtons++;
        if (mouseState.rightButton) mouseButtons++;
        if (mouseState.middleButton) mouseButtons++;
        
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1);
        
        oss << "=== DEBUG MONITOR ===\n\n";
        
        
        oss << "PERFORMANCE\n";
        oss << "  FPS: " << static_cast<int>(std::round(smoothedFps)) << "\n";
        oss << "  Frame Time: " << std::setprecision(2) << (1000.0 / smoothedFps) << " ms\n";
        oss << "  Uptime: " << uptimeMinutes << "m " << uptimeSeconds << "s\n\n";
        
        
        oss << "DISPLAY\n";
        oss << "  Resolution: " << static_cast<int>(screenWidth) << "x" << static_cast<int>(screenHeight) << "\n";
        oss << "  Aspect: " << std::setprecision(2) << (screenWidth / screenHeight) << "\n\n";
        
        
        oss << "MOUSE\n";
        oss << "  Position: (" << std::setprecision(0) << mouseState.x << ", " << mouseState.y << ")\n";
        oss << "  Delta: (" << std::setprecision(1) << (mouseState.x - mouseState.previousX) 
            << ", " << (mouseState.y - mouseState.previousY) << ")\n";
        oss << "  Buttons: " << mouseButtons << " active\n";
        oss << "    L:" << (mouseState.leftButton ? "■" : "□") 
            << " M:" << (mouseState.middleButton ? "■" : "□")
            << " R:" << (mouseState.rightButton ? "■" : "□") << "\n\n";
        
        
        oss << "KEYBOARD\n";
        oss << "  Active Keys: " << activeKeys << "\n";
        
        if (textBox) {
            textBox->setText(oss.str());
            updateSizeFromPrimitives();
        }

        framesAccum = 0;
        accumSeconds = 0.0;
    }

    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    
    getTransform().update(screenWidth, screenHeight);
    simd::float2 pos = getTransform().getAbsolutePosition();
    
    drawPrimitives(encoder);
}
