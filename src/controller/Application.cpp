#include "controller/Application.h"
#include "engine/core/LogManager.h"
#include "engine/systems/MeshRenderer.h"
#include "engine/systems/input/InputState.h"
#include "engine/utils/Math.h"
#include "engine/utils/Path.h"

Application::Application()
{
    LOG_CONSTRUCT("Application");

    EngineConfig config;
    config.nearPlane = 0.5f;
    config.farPlane = 50.0f;
    config.orthographicHeight = 60.0f;
    config.cursorMode = CursorMode::Normal;
    config.exitOnEscape = true;

    config.defaultProjection = ProjectionType::Perspective;
    config.enableDefaultCameraController = true;
    config.cameraPosition = {0.0f, 0.0f, 5.0f};
    config.cameraYawDegrees = 90.0f;
    config.cameraPitchDegrees = -90.0f;
    config.cameraProjectionMode = ProjectionType::Orthographic;
    config.cameraOrthographicHeight = 60.0f;

    engine = std::make_unique<Engine>(config);
    engine->renderer().setClearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0));

    MTL::Device *device = engine->device();

    cubePrimitive = std::make_shared<WorldCubePrimitive>(device, 1.0f, simd::float4{0.6f, 0.8f, 1.0f, 1.0f});
    engine->addRenderable(cubePrimitive->currentRenderable());

    debugMonitor = std::make_shared<DebugMonitor>(device);
    engine->registerUIElement(debugMonitor);

    worldDebugMonitor = std::make_shared<WorldDebugMonitor>(device, 0.5f, 1.0f, 0.0f, 45.0f);
    engine->registerWorldElement(worldDebugMonitor);

    std::string fontPath = Path::dataPath("fonts/Roboto/Roboto-Light.ttf");
    
    TextBoxConfig uiButtonConfig;
    uiButtonConfig.paddingLeft = 20.0f;
    uiButtonConfig.paddingRight = 20.0f;
    uiButtonConfig.paddingTop = 15.0f;
    uiButtonConfig.paddingBottom = 15.0f;
    uiButtonConfig.cornerRadius = 8.0f;
    uiButtonConfig.backgroundColor = simd::float4{0.3f, 0.5f, 0.8f, 1.0f};
    uiButtonConfig.textColor = simd::float4{1.0f, 1.0f, 1.0f, 1.0f};
    uiButtonConfig.autoSizeToContent = true;

    auto uiButton = std::make_shared<UIButtonPrimitive>(device, "Clicks: 0", fontPath, 18.0f, uiButtonConfig);
    uiButton->getTransform().setAnchor(AnchorTarget::Screen, AnchorPoint::BottomLeft, 10.0f, 10.0f);
    uiButton->setCallback([this, uiButton]() {
        clickCount++;
        uiButton->setText("Clicks: " + std::to_string(clickCount));
        LOG_INFO("UI Button clicked! Count: {}", clickCount);
    });
    
    uiButtonContainer = std::make_shared<UIElement>(device);
    uiButtonContainer->addPrimitive(uiButton);
    uiButtonContainer->getTransform().update(InputState::getWindowWidth(), InputState::getWindowHeight());
    engine->registerUIElement(uiButtonContainer);

    TextBoxConfig worldButtonConfig;
    worldButtonConfig.paddingLeft = 15.0f;
    worldButtonConfig.paddingRight = 15.0f;
    worldButtonConfig.paddingTop = 10.0f;
    worldButtonConfig.paddingBottom = 10.0f;
    worldButtonConfig.cornerRadius = 6.0f;
    worldButtonConfig.backgroundColor = simd::float4{0.8f, 0.3f, 0.5f, 1.0f};
    worldButtonConfig.textColor = simd::float4{1.0f, 1.0f, 1.0f, 1.0f};
    
    auto worldButton = std::make_shared<WorldButtonPrimitive>(device, "World Button", -2.0f, 0.0f, 0.0f, 1.5f, 0.5f, fontPath, worldButtonConfig);
    worldButton->setCallback([this]() {
        clickCount++;
        LOG_INFO("World Button clicked! Count: {}", clickCount);
    });
    
    worldButtonContainer = std::make_shared<WorldElement>(device);
    worldButtonContainer->addPrimitive(worldButton);
    engine->registerWorldElement(worldButtonContainer);

    engine->usePerspective(config.perspectiveFovY, config.nearPlane, config.farPlane);
}

Application::~Application()
{
    LOG_DESTROY("Application");
    cubePrimitive.reset();
    debugMonitor.reset();
    worldDebugMonitor.reset();
    uiButtonContainer.reset();
    worldButtonContainer.reset();
    engine.reset();
}

void Application::run()
{
    LOG_START("Application: run starting");

    engine->run([this](Engine::FrameContext &ctx) {
        if (InputState::isKeyPressed(GLFW_KEY_ESCAPE)) {
            engine->stop();
            return;
        }
        
        const float dt = static_cast<float>(ctx.deltaTime);
        cubeRotationDegrees += dt * 25.0f;
        cubePrimitive->setTransform(MetalMath::rotateZ(cubeRotationDegrees));
        ctx.io.set("renderables.cube.rotation.deg", cubeRotationDegrees);
    });

    LOG_FINISH("Application: run finished");
}
