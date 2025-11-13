#include "Application.h"
#include "./lib/GLFW/GLFWAdapter.h"
#include "./lib/Utils/Math.h"
#include "./lib/LogManager/LogManager.h"
#include "./lib/Input/InputState.h"
#include <iostream>
#include <algorithm>
#include "./lib/View/UIContainer.h"
#include "Renderable/UI/FPSMonitor.h"

Application::Application()
{
    LOG_CONSTRUCT("Application");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Treecord", NULL, NULL);
    // Do not hide or capture the cursor; allow normal OS cursor behavior so
    // UI elements (like the FPS monitor) can follow the mouse.
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow*, int width, int height) {
        InputState::setWindowSize(static_cast<float>(width), static_cast<float>(height));
    });

    device = MTL::CreateSystemDefaultDevice();

    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(device);
    metalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    window = get_ns_window(glfwWindow, metalLayer)->retain();

    renderer = new MeshRenderer(device, metalLayer);

    // // --- Example renderables: a cube at world origin and a screen-space square ---
    // // Build a cube mesh and add as world-space renderable
    // Mesh cubeMesh = MeshFactory::buildCube(device);
    // Shader *cubeShader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", cubeMesh.vertexDescriptor);
    // Material *cubeMaterial = new Material(cubeShader);
    // cubeMaterial->setColor({0.2f, 0.7f, 0.3f, 1.0f});
    // Renderable *cubeRenderable = new Renderable(cubeMesh, cubeMaterial);
    // cubeRenderable->setTransform(MetalMath::translate({0.0f, 0.0f, 0.0f}));
    // renderer->addRenderable(cubeRenderable);

    // // Build a screen-space 100x100 quad at (100,100) from top-left
    // // Set renderer ortho params to match the window size (800x600 created above)
    // renderer->setOrthoParams(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    // Mesh screenQuad = MeshFactory::buildScreenQuad(device, 100.0f, 100.0f, 100.0f, 100.0f);
    // Shader *quadShader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", screenQuad.vertexDescriptor);
    // Material *quadMaterial = new Material(quadShader);
    // quadMaterial->setColor({1.0f, 0.2f, 0.2f, 1.0f});
    // Renderable *screenRenderable = new Renderable(screenQuad, quadMaterial);
    // screenRenderable->setScreenSpace(true);
    // renderer->addRenderable(screenRenderable);

    cameraX = -10.0f;
    cameraY = 0.0f;
    cameraZ = 5.0f;

    cameraYaw = 0.0f;
    cameraPitch = 0.0f;

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(glfwWindow, &windowWidth, &windowHeight);
    // Initialize InputState with the actual window dimensions
    InputState::initialize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    // Initialize UI elements
    auto fpsMonitorPtr = std::make_shared<FPSMonitor>(device);
    uiElements.push_back(std::static_pointer_cast<UIContainer>(fpsMonitorPtr));

    // Initialize previous cursor position to current cursor so first-frame deltas are small
    double init_x, init_y;
    glfwGetCursorPos(glfwWindow, &init_x, &init_y);
    prevCursorX = init_x;
    prevCursorY = init_y;
}

Application::~Application()
{
    LOG_DESTROY("Application");
    window->release();
    delete renderer;
    glfwTerminate();

    // Clean up UI elements
    for (auto &uiElement : uiElements)
    {
        uiElement.reset();
    }
}

void Application::run()
{
    LOG_START("Application: run starting");

    double cursor_x, cursor_y;
    float dx, dy;

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraX += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraY += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraY -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraX -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }

        glfwGetCursorPos(glfwWindow, &cursor_x, &cursor_y);

        // Update InputState with current mouse position and button states
        bool leftButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool rightButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        bool middleButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        InputState::update(cursor_x, cursor_y, leftButton, rightButton, middleButton);

        InputState::updateKeyboard(GLFW_KEY_ESCAPE, glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS);

        // Compute deltas from previous cursor position instead of recentering the
        // cursor each frame. This avoids capturing the mouse and lets the OS
        // cursor move freely while still providing motion for the camera.
        dx = -10.0f * static_cast<float>(cursor_x - prevCursorX);
        dy = -10.0f * static_cast<float>(cursor_y - prevCursorY);

        // Update previous cursor position for next frame
        prevCursorX = cursor_x;
        prevCursorY = cursor_y;

        cameraPitch = std::min(89.0f, std::max(-89.0f, cameraPitch + dy));

        cameraYaw = cameraYaw + dx;
        if (cameraYaw < 0.0f)
        {
            cameraYaw += 360.0f;
        }
        if (cameraYaw > 360.0f)
        {
            cameraYaw -= 360.0f;
        }

        simd::float3 forwards = {
            cos(cameraYaw * 3.14159265f / 180.0f) * cos(cameraPitch * 3.14159265f / 180.0f),
            sin(cameraYaw * 3.14159265f / 180.0f) * cos(cameraPitch * 3.14159265f / 180.0f),
            sin(cameraPitch * 3.14159265f / 180.0f)};

        simd::float3 up = {0.0f, 0.0f, 1.0f};
        simd::float3 right = simd::normalize(simd::cross(forwards, up));
        up = simd::normalize(simd::cross(right, forwards));
        simd::float3 pos = {cameraX, cameraY, cameraZ};

        simd::float4x4 view = MetalMath::cameraView(right, up, forwards, pos);

        // Draw scene and UI together in one pass to avoid double-present issues.
        renderer->draw(view, uiElements);
    }
    LOG_FINISH("Application: run finished");
}