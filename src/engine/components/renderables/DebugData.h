#pragma once

struct DebugData {
    int fps = 0;
    double frameTimeMs = 0.0;
    int uptimeMinutes = 0;
    int uptimeSeconds = 0;
    int screenWidth = 0;
    int screenHeight = 0;
    double aspectRatio = 0.0;
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
    int mouseButtonsActive = 0;
    bool leftButton = false;
    bool middleButton = false;
    bool rightButton = false;
    int activeKeys = 0;
};
