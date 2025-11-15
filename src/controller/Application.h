#pragma once

#include "config.h"
#include "engine/Camera.h"

#include <memory>

class Engine;
class Renderable;
class FPSMonitor;
class RenderableTextPrimitive;

class Application
{
public:
    Application();
    ~Application();

    void run();

private:
    std::unique_ptr<Engine> engine;
    std::shared_ptr<Renderable> cubeRenderable;
    std::shared_ptr<FPSMonitor> fpsMonitor;
    std::shared_ptr<RenderableTextPrimitive> worldText;

    float cubeRotationDegrees = 0.0f;
};
