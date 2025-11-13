#include "controller/Application.h"
#include "backend/glfw/GLFWAdapter.h"
#include "utils/Math.h"
#include "core/LogManager.h"
#include "systems/input/InputState.h"
#include <iostream>
#include <algorithm>
#include "components/renderables/core/UIContainer.h"
#include "components/renderables/ui/FPSMonitor.h"

Application::Application()
{
    LOG_CONSTRUCT("Application");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Treecord", NULL, NULL);
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

    cameraPositionX = -10.0f;
    cameraPositionY = 0.0f;
    cameraPositionZ = 5.0f;

    cameraYaw = 0.0f;
    cameraPitch = 0.0f;

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(glfwWindow, &windowWidth, &windowHeight);
    InputState::initialize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    auto fpsMonitorPtr = std::make_shared<FPSMonitor>(device);
    uiElements.push_back(std::static_pointer_cast<UIContainer>(fpsMonitorPtr));

    double initialCursorX, initialCursorY;
    glfwGetCursorPos(glfwWindow, &initialCursorX, &initialCursorY);
    previousCursorX = initialCursorX;
    previousCursorY = initialCursorY;
}

Application::~Application()
{
    LOG_DESTROY("Application");
    window->release();
    delete renderer;
    glfwTerminate();

    for (auto &uiElement : uiElements)
    {
        uiElement.reset();
    }
}

void Application::run()
{
    LOG_START("Application: run starting");

    double cursorX, cursorY;
    float deltaX, deltaY;

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPositionX += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPositionY += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPositionY -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPositionX -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }

        glfwGetCursorPos(glfwWindow, &cursorX, &cursorY);

        bool leftButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool rightButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        bool middleButton = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        InputState::update(cursorX, cursorY, leftButton, rightButton, middleButton);

        InputState::updateKeyboard(GLFW_KEY_ESCAPE, glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS);

        deltaX = -10.0f * static_cast<float>(cursorX - previousCursorX);
        deltaY = -10.0f * static_cast<float>(cursorY - previousCursorY);

        previousCursorX = cursorX;
        previousCursorY = cursorY;

        cameraPitch = std::min(89.0f, std::max(-89.0f, cameraPitch + deltaY));

        cameraYaw = cameraYaw + deltaX;
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
        simd::float3 pos = {cameraPositionX, cameraPositionY, cameraPositionZ};

        simd::float4x4 view = MetalMath::cameraView(right, up, forwards, pos);

        renderer->draw(view, uiElements);
    }
    LOG_FINISH("Application: run finished");
}
