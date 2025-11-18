#pragma once

#include "engine/config.h"
#include "engine/core/Camera.h"

#include "engine/core/Engine.h"
#include "engine/components/renderables/primitives/3d/WorldCubePrimitive.h"
#include "controller/ui/DebugMonitor.h"
#include "controller/world/WorldDebugMonitor.h"
#include "engine/components/renderables/primitives/ui/UIButtonPrimitive.h"
#include "engine/components/renderables/primitives/3d/WorldButtonPrimitive.h"
#include "engine/components/renderables/core/UIElement.h"
#include "engine/components/renderables/core/WorldElement.h"

#include <memory>

class Application
{
public:
    Application();
    ~Application();

    void run();

private:
    std::unique_ptr<Engine> engine;
    std::shared_ptr<WorldCubePrimitive> cubePrimitive;
    std::shared_ptr<DebugMonitor> debugMonitor;
    std::shared_ptr<WorldDebugMonitor> worldDebugMonitor;
    std::shared_ptr<UIElement> uiButtonContainer;
    std::shared_ptr<WorldElement> worldButtonContainer;

    float cubeRotationDegrees = 0.0f;
    int clickCount = 0;
};
