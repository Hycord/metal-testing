#include "controller/Application.h"
#include "engine/Engine.h"
#include "components/engine/Material.h"
#include "components/engine/Renderable.h"
#include "components/engine/Shader.h"
#include "components/renderables/ui/FPSMonitor.h"
#include "core/LogManager.h"
#include "factories/MeshFactory.h"
#include "systems/MeshRenderer.h"
#include "utils/Math.h"

Application::Application()
{
    LOG_CONSTRUCT("Application");

    EngineConfig config;
    config.nearPlane = 0.1f;
    config.farPlane = 400.0f;
    config.orthographicHeight = 60.0f;
    config.cursorMode = CursorMode::Normal;
    config.exitOnEscape = true;

    config.defaultProjection = ProjectionType::Perspective;
    config.enableDefaultCameraController = false;
    config.cameraPosition = {0.0f, -6.0f, 3.0f};
    config.cameraYawDegrees = 90.0f;
    config.cameraPitchDegrees = -25.0f;
    config.cameraProjectionMode = ProjectionType::Perspective;
    config.cameraOrthographicHeight = 60.0f;

    engine = std::make_unique<Engine>(config);
    engine->renderer().setClearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0));

    MTL::Device *device = engine->device();

    Mesh cubeMesh = MeshFactory::buildCube(device);
    auto shader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", cubeMesh.vertexDescriptor);
    auto material = new Material(shader);
    material->setColor(simd::float4{0.6f, 0.8f, 1.0f, 1.0f});
    cubeRenderable = std::make_shared<Renderable>(cubeMesh, material);
    engine->addRenderable(cubeRenderable);

    fpsMonitor = std::make_shared<FPSMonitor>(device);
    engine->registerUIElement(fpsMonitor);

    engine->usePerspective(config.perspectiveFovY, config.nearPlane, config.farPlane);
}

Application::~Application()
{
    LOG_DESTROY("Application");
}

void Application::run()
{
    LOG_START("Application: run starting");

    engine->run([this](Engine::FrameContext &ctx) {
        const float dt = static_cast<float>(ctx.deltaTime);
        cubeRotationDegrees += dt * 25.0f;
        cubeRenderable->setTransform(MetalMath::rotateZ(cubeRotationDegrees));
        ctx.io.set("renderables.cube.rotation.deg", cubeRotationDegrees);
    });

    LOG_FINISH("Application: run finished");
}
