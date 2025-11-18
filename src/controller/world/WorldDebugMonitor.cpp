#include "controller/world/WorldDebugMonitor.h"
#include "engine/core/LogManager.h"
#include "engine/systems/input/InputState.h"
#include "engine/utils/Path.h"
#include "engine/utils/Math.h"
#include <cmath>
#include <sstream>
#include <iomanip>

WorldDebugMonitor::WorldDebugMonitor(MTL::Device* device,
                                     float x, float y, float z,
                                     float pitch,
                                     float yaw,
                                     float roll)
    : WorldElement(device)
{
    LOG_CONSTRUCT("WorldDebugMonitor");
    
    setPosition(x, y, z);
    setRotation(pitch, yaw, roll);
    
    std::string fontPath = Path::dataPath("fonts/Roboto/Roboto-Light.ttf");
    
    TextBoxConfig config;
    config.paddingLeft = 20.0f;
    config.paddingRight = 20.0f;
    config.paddingTop = 20.0f;
    config.paddingBottom = 20.0f;
    config.cornerRadius = 8.0f;
    config.cornerSegments = 16;
    config.backgroundColor = simd::float4{0.10f, 0.10f, 0.10f, 0.85f};
    config.textColor = simd::float4{0.9f, 0.9f, 0.9f, 1.0f};
    config.enableWordWrap = false;
    config.autoSizeToContent = false;
    
    float worldWidth = 2.0f;
    float worldHeight = 2.0f;
    
    std::string initialText = formatDebugText(createSizingDefaults());
    
    textBox = std::make_shared<WorldTextBoxPrimitive>(
        device,
        initialText,
        x, y, z,
        worldWidth, worldHeight,
        fontPath,
        config
    );
    
    float cosP = std::cos(pitch);
    float sinP = std::sin(pitch);
    float cosY = std::cos(yaw);
    float sinY = std::sin(yaw);
    
    simd::float3 forward = simd::normalize(simd::float3{
        cosP * sinY,
        -sinP,
        cosP * cosY
    });
    
    simd::float3 up = simd::normalize(simd::float3{
        sinP * sinY,
        cosP,
        sinP * cosY
    });
    
    textBox->setOrientation(forward, up);
    
    addPrimitive(textBox);
    
    LOG_INFO("WorldDebugMonitor initialized at position ({}, {}, {}) with rotation ({}, {}, {})",
             x, y, z, pitch, yaw, roll);
}

WorldDebugMonitor::~WorldDebugMonitor()
{
    LOG_DESTROY("WorldDebugMonitor");
}

void WorldDebugMonitor::render(MTL::RenderCommandEncoder* encoder,
                                const simd::float4x4& projection,
                                const simd::float4x4& view)
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

        framesAccum = 0;
        accumSeconds = 0.0;
    }
    
    drawPrimitives(encoder, projection, view);
}

void WorldDebugMonitor::setMessage(const std::string& message)
{
    if (textBox) {
        textBox->setText(message);
    }
}

DebugData WorldDebugMonitor::createSizingDefaults()
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

DebugData WorldDebugMonitor::buildDebugData() const
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

std::string WorldDebugMonitor::formatDebugText(const DebugData& data) const
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
