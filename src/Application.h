#pragma once

#include "config.h"
#include "./lib/View/MeshRenderer.h"
#include "./lib/View/Renderable.h"
#include "./lib/View/UIContainer.h"
#include <vector>
#include <memory>

class Application {
    public:
        Application();
        ~Application();

        void run();

    private:
        GLFWwindow* glfwWindow;
        MTL::Device* device;
        CA::MetalLayer* metalLayer;
        NS::Window* window;
    MeshRenderer* renderer;

        float cameraX, cameraY, cameraZ;
        float cameraPitch, cameraYaw;

    std::vector<std::shared_ptr<UIContainer>> uiElements;
    // Keep a typed pointer to the FPS monitor so we can update its position from
    // the main loop without dynamic_casting every frame.
    class FPSMonitor* fpsMonitor;

    // Track previous cursor position so we can compute deltas when the cursor
    // is not being recentered (we no longer capture the cursor).
    double prevCursorX = 0.0;
    double prevCursorY = 0.0;
};