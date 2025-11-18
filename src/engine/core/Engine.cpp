#include "engine/core/Engine.h"

#include "engine/backend/glfw/GLFWAdapter.h"
#include "engine/components/engine/Renderable.h"
#include "engine/components/renderables/core/UIContainer.h"
#include "engine/components/renderables/core/WorldContainer.h"
#include "engine/core/LogManager.h"
#include "engine/core/FontManager.h"
#include "engine/systems/MeshRenderer.h"
#include "engine/systems/input/InputState.h"

#include <QuartzCore/CAMetalLayer.hpp>

#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace {
int to_glfw_cursor_mode(CursorMode mode) {
    switch (mode) {
        case CursorMode::Hidden:   return GLFW_CURSOR_HIDDEN;
        case CursorMode::Disabled: return GLFW_CURSOR_DISABLED;
        case CursorMode::Normal:
        default:                   return GLFW_CURSOR_NORMAL;
    }
}

constexpr float kDegreesToRadians = static_cast<float>(M_PI / 180.0);
}

Engine::Engine(const EngineConfig &config)
        : defaultProjectionMode_(config.defaultProjection),
            perspectiveFovY_(config.perspectiveFovY),
            nearPlane_(config.nearPlane),
            farPlane_(config.farPlane),
            orthographicHeight_(config.orthographicHeight),
            exitOnEscape_(config.exitOnEscape),
            cursorMode_(config.cursorMode)
{
    LOG_CONSTRUCT("Engine");

    initializeGlfw(config);

    device_ = MTL::CreateSystemDefaultDevice();
    if (!device_) {
        LOG_ERROR("Engine: failed to acquire Metal device");
        throw std::runtime_error("Failed to create Metal device");
    }
    
    
    FontManager::getInstance().initialize(device_);

    metalLayer_ = CA::MetalLayer::layer()->retain();
    metalLayer_->setDevice(device_);
    metalLayer_->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    nsWindow_ = get_ns_window(window_, metalLayer_)->retain();

    renderer_ = std::make_unique<MeshRenderer>(device_, metalLayer_);

    int winWidth = 0;
    int winHeight = 0;
    glfwGetWindowSize(window_, &winWidth, &winHeight);
    windowWidth_ = static_cast<float>(winWidth);
    windowHeight_ = static_cast<float>(winHeight);
    InputState::initialize(windowWidth_, windowHeight_);

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(window_, &fbWidth, &fbHeight);
    framebufferWidth_ = static_cast<float>(fbWidth);
    framebufferHeight_ = static_cast<float>(fbHeight);
    updateDrawableSize();

    if (renderer_) {
        renderer_->setOrthoParams(0.0f, windowWidth_, 0.0f, windowHeight_, -1.0f, 1.0f);
    }

    projectionToggleEnabled_ = config.enableDefaultCameraController;
    orthoScaleKeysEnabled_ = config.enableDefaultCameraController;

    cameraController_.enabled = config.enableDefaultCameraController;
    cameraController_.position = config.cameraPosition;
    cameraController_.yawDegrees = config.cameraYawDegrees;
    cameraController_.pitchDegrees = config.cameraPitchDegrees;
    cameraController_.moveSpeed = config.cameraMoveSpeed;
    cameraController_.mouseSensitivity = config.cameraMouseSensitivity;
    cameraController_.projection = config.cameraProjectionMode;
    cameraController_.orthoHeight = config.cameraOrthographicHeight;

    defaultProjectionMode_ = cameraController_.projection;
    orthographicHeight_ = config.cameraOrthographicHeight;

    baseView_ = MetalMath::identity();
    cameraMatrices_.view = baseView_;
    updateProjectionMatrix();
    updateCamera(0.0);

    lastFrameTime_ = glfwGetTime();
}

Engine::~Engine()
{
    LOG_DESTROY("Engine");
    uiElements_.clear();
    worldElements_.clear();
    renderer_.reset();
    FontManager::getInstance().shutdown();
    if (metalLayer_) {
        metalLayer_->release();
        metalLayer_ = nullptr;
    }
    if (device_) {
        device_->release();
        device_ = nullptr;
    }
    if (window_) {
        glfwSetWindowUserPointer(window_, nullptr);
        glfwSetWindowSizeCallback(window_, nullptr);
        glfwSetFramebufferSizeCallback(window_, nullptr);
        glfwSetKeyCallback(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (nsWindow_) {
        nsWindow_->release();
        nsWindow_ = nullptr;
    }
    glfwTerminate();
}

void Engine::initializeGlfw(const EngineConfig &config)
{
    if (!glfwInit()) {
        LOG_ERROR("Engine: GLFW initialization failed");
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(static_cast<int>(config.windowWidth),
                               static_cast<int>(config.windowHeight),
                               config.windowTitle.c_str(),
                               nullptr,
                               nullptr);
    if (!window_) {
        LOG_ERROR("Engine: failed to create GLFW window");
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetInputMode(window_, GLFW_CURSOR, to_glfw_cursor_mode(cursorMode_));

    installCallbacks();
}

void Engine::installCallbacks()
{
    glfwSetWindowSizeCallback(window_, [](GLFWwindow *win, int width, int height) {
        if (auto *engine = static_cast<Engine*>(glfwGetWindowUserPointer(win))) {
            engine->onWindowSizeChanged(width, height);
        }
    });

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *win, int width, int height) {
        if (auto *engine = static_cast<Engine*>(glfwGetWindowUserPointer(win))) {
            engine->onFramebufferSizeChanged(width, height);
        }
    });

    glfwSetKeyCallback(window_, [](GLFWwindow *win, int key, int, int action, int) {
        if (auto *engine = static_cast<Engine*>(glfwGetWindowUserPointer(win))) {
            const bool pressed = action != GLFW_RELEASE;
            engine->onKeyEvent(key, pressed);
        }
    });
}

void Engine::run(const FrameCallback &frameCallback)
{
    shouldClose_ = false;
    while (pumpFrame(frameCallback)) {
        
    }
}

bool Engine::pumpFrame(const FrameCallback &frameCallback)
{
    if (!window_ || !renderer_) {
        return false;
    }

    if (shouldClose_ || glfwWindowShouldClose(window_)) {
        return false;
    }

    glfwPollEvents();
    updateInputState();

    const double now = glfwGetTime();
    const double delta = now - lastFrameTime_;
    lastFrameTime_ = now;

    updateCamera(delta);

    ioChannel_.set("time.absolute", now);
    ioChannel_.set("time.delta", delta);
    ioChannel_.set("window.width", windowWidth_);
    ioChannel_.set("window.height", windowHeight_);
    ioChannel_.set("camera.position", cameraController_.position);
    ioChannel_.set("camera.yaw.deg", cameraController_.yawDegrees);
    ioChannel_.set("camera.pitch.deg", cameraController_.pitchDegrees);

    FrameContext context{now, delta, ioChannel_, window_, *renderer_, *this, cameraMatrices_};

    if (frameCallback) {
        frameCallback(context);
    }

    renderer_->draw(cameraMatrices_, uiElements_, worldElements_);

    return !(shouldClose_ || glfwWindowShouldClose(window_));
}

void Engine::stop()
{
    shouldClose_ = true;
    if (window_) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

void Engine::addRenderable(const std::shared_ptr<Renderable> &renderable)
{
    if (renderer_ && renderable) {
        renderer_->addRenderable(renderable);
    }
}

void Engine::clearRenderables()
{
    if (renderer_) {
        renderer_->clearRenderables();
    }
}

void Engine::registerUIElement(const std::shared_ptr<UIContainer> &element)
{
    if (!element) {
        return;
    }
    uiElements_.push_back(element);
}

void Engine::clearUI()
{
    uiElements_.clear();
}

void Engine::registerWorldElement(const std::shared_ptr<WorldContainer> &element)
{
    if (!element) {
        return;
    }
    worldElements_.push_back(element);
}

void Engine::clearWorld()
{
    worldElements_.clear();
}

MeshRenderer& Engine::renderer()
{
    return *renderer_;
}

const MeshRenderer& Engine::renderer() const
{
    return *renderer_;
}

float Engine::aspectRatio() const
{
    return (windowHeight_ > 0.0f) ? (windowWidth_ / windowHeight_) : 1.0f;
}

void Engine::usePerspective(float fovDegrees, float nearPlane, float farPlane)
{
    perspectiveFovY_ = fovDegrees;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;
    defaultProjectionMode_ = ProjectionType::Perspective;
    cameraController_.projection = ProjectionType::Perspective;
    updateProjectionMatrix();
}

void Engine::useOrthographic(float verticalSize, float nearPlane, float farPlane)
{
    orthographicHeight_ = verticalSize;
    cameraController_.orthoHeight = verticalSize;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;
    defaultProjectionMode_ = ProjectionType::Orthographic;
    cameraController_.projection = ProjectionType::Orthographic;
    updateProjectionMatrix();
}

void Engine::useCustomProjection(const simd::float4x4 &projection)
{
    customProjection_ = projection;
    defaultProjectionMode_ = ProjectionType::Custom;
    cameraController_.projection = ProjectionType::Custom;
    updateProjectionMatrix();
}

void Engine::setDefaultView(const simd::float4x4 &view)
{
    baseView_ = view;
    baseViewOverride_ = true;
    if (!cameraController_.enabled) {
        cameraMatrices_.view = view;
    }
}

void Engine::updateInputState()
{
    if (!window_) {
        return;
    }
    double cursorX = 0.0;
    double cursorY = 0.0;
    glfwGetCursorPos(window_, &cursorX, &cursorY);

    const bool left = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool right = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    const bool middle = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

    InputState::update(cursorX, cursorY, left, right, middle);
}

void Engine::onWindowSizeChanged(int width, int height)
{
    windowWidth_ = static_cast<float>(width);
    windowHeight_ = static_cast<float>(height);
    InputState::setWindowSize(windowWidth_, windowHeight_);
    if (renderer_) {
        renderer_->setOrthoParams(0.0f, windowWidth_, 0.0f, windowHeight_, -1.0f, 1.0f);
    }
    updateProjectionMatrix();
}

void Engine::onFramebufferSizeChanged(int width, int height)
{
    framebufferWidth_ = static_cast<float>(width);
    framebufferHeight_ = static_cast<float>(height);
    updateDrawableSize();
}

void Engine::onKeyEvent(int key, bool pressed)
{
    if (key >= 0 && key < GLFW_KEY_LAST) {
        InputState::updateKeyboard(key, pressed);
    }
}

void Engine::updateDrawableSize()
{
    if (!metalLayer_) {
        return;
    }
    CGSize size;
    size.width = static_cast<CGFloat>(framebufferWidth_);
    size.height = static_cast<CGFloat>(framebufferHeight_);
    metalLayer_->setDrawableSize(size);
}

void Engine::updateCamera(double deltaSeconds)
{
    const bool controllerEnabled = cameraController_.enabled;
    if (!controllerEnabled) {
        cameraController_.projection = defaultProjectionMode_;
    }

    if (controllerEnabled) {
        const float dt = static_cast<float>(deltaSeconds);
        const float mouseDeltaX = static_cast<float>(InputState::getMouseDeltaX());
        const float mouseDeltaY = static_cast<float>(InputState::getMouseDeltaY());

        cameraController_.yawDegrees -= mouseDeltaX * cameraController_.mouseSensitivity;
        cameraController_.pitchDegrees -= mouseDeltaY * cameraController_.mouseSensitivity;
    }

    cameraController_.pitchDegrees = std::clamp(cameraController_.pitchDegrees, -89.0f, 89.0f);

    if (cameraController_.yawDegrees < 0.0f) {
        cameraController_.yawDegrees += 360.0f;
    } else if (cameraController_.yawDegrees >= 360.0f) {
        cameraController_.yawDegrees -= 360.0f;
    }

    if (controllerEnabled && projectionToggleEnabled_) {
        if (InputState::isKeyPressed(GLFW_KEY_1)) {
            cameraController_.projection = ProjectionType::Perspective;
        }
        if (InputState::isKeyPressed(GLFW_KEY_2)) {
            cameraController_.projection = ProjectionType::Orthographic;
        }
    }

    const float yawRad = cameraController_.yawDegrees * kDegreesToRadians;
    const float pitchRad = cameraController_.pitchDegrees * kDegreesToRadians;

    simd::float3 forwards = {
        cosf(yawRad) * cosf(pitchRad),
        sinf(yawRad) * cosf(pitchRad),
        sinf(pitchRad)
    };
    forwards = simd::normalize(forwards);

    const simd::float3 worldUp = {0.0f, 0.0f, 1.0f};
    simd::float3 right = simd::normalize(simd::cross(forwards, worldUp));
    simd::float3 up = simd::normalize(simd::cross(right, forwards));

    if (controllerEnabled) {
        const float dt = static_cast<float>(deltaSeconds);
        const float velocity = cameraController_.moveSpeed * dt;
        if (InputState::isKeyPressed(GLFW_KEY_W)) cameraController_.position += forwards * velocity;
        if (InputState::isKeyPressed(GLFW_KEY_S)) cameraController_.position -= forwards * velocity;
        if (InputState::isKeyPressed(GLFW_KEY_D)) cameraController_.position += right * velocity;
        if (InputState::isKeyPressed(GLFW_KEY_A)) cameraController_.position -= right * velocity;
        if (InputState::isKeyPressed(GLFW_KEY_SPACE)) cameraController_.position += worldUp * velocity;
        if (InputState::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) cameraController_.position -= worldUp * velocity;

        if (cameraController_.projection == ProjectionType::Orthographic && orthoScaleKeysEnabled_) {
            if (InputState::isKeyPressed(GLFW_KEY_EQUAL)) {
                cameraController_.orthoHeight = std::max(5.0f, cameraController_.orthoHeight - 40.0f * dt);
            }
            if (InputState::isKeyPressed(GLFW_KEY_MINUS)) {
                cameraController_.orthoHeight = std::min(200.0f, cameraController_.orthoHeight + 40.0f * dt);
            }
        }
    }

    if (!controllerEnabled) {
        cameraController_.orthoHeight = orthographicHeight_;
    } else if (cameraController_.projection == ProjectionType::Orthographic) {
        orthographicHeight_ = cameraController_.orthoHeight;
    }

    const simd::float4x4 viewMatrix = baseViewOverride_ ? baseView_ : MetalMath::cameraView(right, up, forwards, cameraController_.position);
    cameraMatrices_.view = viewMatrix;

    updateProjectionMatrix();
}

void Engine::updateProjectionMatrix()
{
    float aspect = aspectRatio();
    switch (cameraController_.projection) {
        case ProjectionType::Perspective:
            cameraMatrices_.projection = MetalMath::perspectiveProjection(perspectiveFovY_, aspect, nearPlane_, farPlane_);
            break;
        case ProjectionType::Orthographic: {
            const float halfHeight = cameraController_.orthoHeight * 0.5f;
            const float halfWidth = halfHeight * aspect;
            cameraMatrices_.projection = MetalMath::orthographicProjection(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane_, farPlane_);
            break;
        }
        case ProjectionType::Custom:
        default:
            cameraMatrices_.projection = customProjection_;
            break;
    }
    cameraMatrices_.projectionType = cameraController_.projection;
}

const simd::float3& Engine::cameraPosition() const
{
    return cameraController_.position;
}

float Engine::cameraYawDegrees() const
{
    return cameraController_.yawDegrees;
}

float Engine::cameraPitchDegrees() const
{
    return cameraController_.pitchDegrees;
}
