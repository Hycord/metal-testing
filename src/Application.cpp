#include "Application.h"
#include "./lib/GLFW/GLFWAdapter.h"
#include "./lib/Utils/Math.h"
#include "./lib/LogManager/LogManager.h"
#include <iostream>

Application::Application()
{
    LOG_CONSTRUCT("Application");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Treecord", NULL, NULL);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    device = MTL::CreateSystemDefaultDevice();

    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(device);
    metalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    window = get_ns_window(glfwWindow, metalLayer)->retain();

    renderer = new MeshRenderer(device, metalLayer);

    cameraX = -10.0f;
    cameraY = 0.0f;
    cameraZ = 5.0f;

    cameraYaw = 0.0f;
    cameraPitch = 0.0f;
}

Application::~Application()
{
    LOG_DESTROY("Application");
    window->release();
    delete renderer;
    glfwTerminate();
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
        dx = -10.0f * static_cast<float>(cursor_x / 400.0 - 1.0);
        dy = -10.0f * static_cast<float>(cursor_y / 300.0 - 1.0);
        glfwSetCursorPos(glfwWindow, 400.0, 300.0);

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

        renderer->draw(view);
    }
    LOG_FINISH("Application: run finished");
}