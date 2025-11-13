#pragma once

#include "config.h"
#include "systems/MeshRenderer.h"
#include "components/engine/Renderable.h"
#include "components/renderables/core/UIContainer.h"
#include <vector>
#include <memory>

class Application
{
public:
    Application();
    ~Application();

    void run();

private:
    GLFWwindow *glfwWindow;
    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    NS::Window *window;
    MeshRenderer *renderer;

    float cameraPositionX, cameraPositionY, cameraPositionZ;
    float cameraPitch, cameraYaw;

    std::vector<std::shared_ptr<UIContainer>> uiElements;

    double previousCursorX = 0.0;
    double previousCursorY = 0.0;
};
