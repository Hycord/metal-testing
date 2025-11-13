#pragma once

#include "config.h"
#include "./lib/View/MeshRenderer.h"

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
};