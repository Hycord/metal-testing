#pragma once

#include "../../config.h"
#include "../View/Renderer.h"

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
        Renderer* renderer;

        float cameraX, cameraY, cameraZ;
        float cameraPitch, cameraYaw;
};