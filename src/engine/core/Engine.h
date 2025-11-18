#pragma once

#include "engine/config.h"
#include "engine/core/Camera.h"
#include "engine/core/EngineIO.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class MeshRenderer;
class Renderable;
class UIContainer;
class WorldContainer;

enum class CursorMode {
    Normal,
    Hidden,
    Disabled
};

struct EngineConfig {
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    std::string windowTitle = "Renderer";
    ProjectionType defaultProjection = ProjectionType::Perspective;
    float perspectiveFovY = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float orthographicHeight = 20.0f;
    CursorMode cursorMode = CursorMode::Normal;
    bool exitOnEscape = true;
    bool enableDefaultCameraController = true;
    simd::float3 cameraPosition = {0.0f, 0.0f, 5.0f};
    float cameraYawDegrees = 0.0f;
    float cameraPitchDegrees = 0.0f;
    float cameraMoveSpeed = 6.0f;
    float cameraMouseSensitivity = 0.1f;
    ProjectionType cameraProjectionMode = ProjectionType::Perspective;
    float cameraOrthographicHeight = 20.0f;
};

class Engine {
public:
    struct FrameContext {
        double absoluteTime;
        double deltaTime;
        EngineIO &io;
        GLFWwindow *window;
        MeshRenderer &renderer;
        Engine &engine;
        const CameraMatrices &camera;
    };

    using FrameCallback = std::function<void(FrameContext&)>;

    explicit Engine(const EngineConfig &config);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void run(const FrameCallback &frameCallback);
    bool pumpFrame(const FrameCallback &frameCallback);

    void stop();

    void addRenderable(const std::shared_ptr<Renderable> &renderable);
    void clearRenderables();

    void registerUIElement(const std::shared_ptr<UIContainer> &element);
    void clearUI();

    void registerWorldElement(const std::shared_ptr<WorldContainer> &element);
    void clearWorld();

    EngineIO& io() { return ioChannel_; }
    const EngineIO& io() const { return ioChannel_; }

    MTL::Device* device() const { return device_; }
    MeshRenderer& renderer();
    const MeshRenderer& renderer() const;

    GLFWwindow* window() const { return window_; }

    float aspectRatio() const;

    void usePerspective(float fovDegrees, float nearPlane, float farPlane);
    void useOrthographic(float verticalSize, float nearPlane, float farPlane);
    void useCustomProjection(const simd::float4x4 &projection);
    void setDefaultView(const simd::float4x4 &view);

    const CameraMatrices& camera() const { return cameraMatrices_; }
    const simd::float3& cameraPosition() const;
    float cameraYawDegrees() const;
    float cameraPitchDegrees() const;

    float windowWidth() const { return windowWidth_; }
    float windowHeight() const { return windowHeight_; }
    float framebufferWidth() const { return framebufferWidth_; }
    float framebufferHeight() const { return framebufferHeight_; }

private:
    void initializeGlfw(const EngineConfig &config);
    void installCallbacks();
    void updateInputState();
    void updateCamera(double deltaSeconds);
    void updateProjectionMatrix();
    void onWindowSizeChanged(int width, int height);
    void onFramebufferSizeChanged(int width, int height);
    void onKeyEvent(int key, bool pressed);
    void updateDrawableSize();

    struct CameraControllerState {
        bool enabled = true;
        simd::float3 position = {0.0f, 0.0f, 0.0f};
        float yawDegrees = 0.0f;
        float pitchDegrees = 0.0f;
        float moveSpeed = 6.0f;
        float mouseSensitivity = 0.1f;
        float orthoHeight = 20.0f;
        ProjectionType projection = ProjectionType::Perspective;
    };

    GLFWwindow *window_ = nullptr;
    MTL::Device *device_ = nullptr;
    CA::MetalLayer *metalLayer_ = nullptr;
    NS::Window *nsWindow_ = nullptr;
    std::unique_ptr<MeshRenderer> renderer_;
    std::vector<std::shared_ptr<UIContainer>> uiElements_;
    std::vector<std::shared_ptr<WorldContainer>> worldElements_;
    EngineIO ioChannel_;

    ProjectionType defaultProjectionMode_ = ProjectionType::Perspective;
    float perspectiveFovY_ = 60.0f;
    float nearPlane_ = 0.1f;
    float farPlane_ = 100.0f;
    float orthographicHeight_ = 20.0f;
    simd::float4x4 baseView_ = MetalMath::identity();
    bool baseViewOverride_ = false;
    simd::float4x4 customProjection_ = MetalMath::identity();
    CameraControllerState cameraController_{};
    CameraMatrices cameraMatrices_;
    bool projectionToggleEnabled_ = true;
    bool orthoScaleKeysEnabled_ = true;

    double lastFrameTime_ = 0.0;
    bool shouldClose_ = false;
    bool exitOnEscape_ = true;
    CursorMode cursorMode_ = CursorMode::Normal;

    float windowWidth_ = 0.0f;
    float windowHeight_ = 0.0f;
    float framebufferWidth_ = 0.0f;
    float framebufferHeight_ = 0.0f;
};
