#include "controller/ui/DebugMonitor.h"
#include "engine/components/renderables/primitives/ui/UITextBoxPrimitive.h"
#include "engine/factories/MeshFactory.h"
#include "engine/components/engine/Shader.h"
#include "engine/core/LogManager.h"
#include "engine/systems/input/InputState.h"
#include "engine/utils/Path.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>

DebugMonitor::DebugMonitor(MTL::Device *device)
    : UIElement(device)
{
    const float marginX = 10.0f;
    const float marginY = 10.0f;

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
    config.autoSizeToContent = true;
    config.maxWidth = 400.0f;
    
    std::string initialText = formatDebugText(createSizingDefaults());
    
    textBox = std::make_shared<UITextBoxPrimitive>(
        device,
        initialText,
        fontPath,
        fontSize,
        config
    );
    
    textBox->getTransform().setParent(&getTransform());
    textBox->getTransform().setAnchor(AnchorTarget::Parent, AnchorPoint::TopRight, 0.0f, 0.0f);
    
    addPrimitive(textBox);

    enableAutoAnchor(AnchorCorner::TopRight, marginX, marginY);
    updateSizeFromPrimitives();
}

DebugMonitor::~DebugMonitor()
{
    LOG_DESTROY("DebugMonitor");
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
        smoothedFps = (smoothedFps <= 0.0) ? instFps : smoothedFps * 0.8 + instFps * 0.2;
        
        textBox->setText(formatDebugText(buildDebugData()));
        updateSizeFromPrimitives();

        framesAccum = 0;
        accumSeconds = 0.0;
    }

    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    getTransform().update(screenWidth, screenHeight);
    
    drawPrimitives(encoder);
}

DebugData DebugMonitor::createSizingDefaults()
{
    DebugData data;
    data.fps = 999;
    data.frameTimeMs = 99.99;
    data.uptimeMinutes = 99;
    data.uptimeSeconds = 59;
    data.screenWidth = 9999;
    data.screenHeight = 9999;
    data.aspectRatio = 9.99;
    data.mouseX = 9999.0f;
    data.mouseY = 9999.0f;
    data.mouseDeltaX = 999.9f;
    data.mouseDeltaY = 999.9f;
    data.mouseButtonsActive = 3;
    data.leftButton = true;
    data.middleButton = true;
    data.rightButton = true;
    data.activeKeys = 99;
    return data;
}

DebugData DebugMonitor::buildDebugData() const
{
    const auto& mouseState = InputState::getMouseState();
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    
    const double uptime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
    const int uptimeInt = static_cast<int>(uptime);
    
    const auto& keyState = InputState::getKeyboardState();
    int activeKeys = 0;
    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        activeKeys += keyState.keys[i];
    }
    
    const int mouseButtons = mouseState.leftButton + mouseState.rightButton + mouseState.middleButton;
    
    DebugData data;
    data.fps = static_cast<int>(std::round(smoothedFps));
    data.frameTimeMs = 1000.0 / smoothedFps;
    data.uptimeMinutes = uptimeInt / 60;
    data.uptimeSeconds = uptimeInt % 60;
    data.screenWidth = static_cast<int>(screenWidth);
    data.screenHeight = static_cast<int>(screenHeight);
    data.aspectRatio = screenWidth / screenHeight;
    data.mouseX = mouseState.x;
    data.mouseY = mouseState.y;
    data.mouseDeltaX = mouseState.x - mouseState.previousX;
    data.mouseDeltaY = mouseState.y - mouseState.previousY;
    data.mouseButtonsActive = mouseButtons;
    data.leftButton = mouseState.leftButton;
    data.middleButton = mouseState.middleButton;
    data.rightButton = mouseState.rightButton;
    data.activeKeys = activeKeys;
    return data;
}

std::string DebugMonitor::formatDebugText(const DebugData& data) const
{
    std::ostringstream oss;
    oss << std::fixed
        << "=== DEBUG MONITOR ===\n\n"
        << "PERFORMANCE\n"
        << "  FPS: " << data.fps << "\n"
        << "  Frame Time: " << std::setprecision(2) << data.frameTimeMs << " ms\n"
        << "  Uptime: " << data.uptimeMinutes << "m " << data.uptimeSeconds << "s\n\n"
        << "DISPLAY\n"
        << "  Resolution: " << data.screenWidth << "x" << data.screenHeight << "\n"
        << "  Aspect: " << data.aspectRatio << "\n\n"
        << "MOUSE\n"
        << "  Position: (" << std::setprecision(0) << data.mouseX << ", " << data.mouseY << ")\n"
        << "  Delta: (" << std::setprecision(1) << data.mouseDeltaX << ", " << data.mouseDeltaY << ")\n"
        << "  Buttons: " << data.mouseButtonsActive << " active\n"
        << "    L:" << (data.leftButton ? "■" : "□")
        << " M:" << (data.middleButton ? "■" : "□")
        << " R:" << (data.rightButton ? "■" : "□") << "\n\n"
        << "KEYBOARD\n"
        << "  Active Keys: " << data.activeKeys << "\n";
    
    return oss.str();
}
