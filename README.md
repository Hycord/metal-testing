# Metal Renderer Starter (Jump-Off Point)

C++20 Metal-based renderer for macOS designed as a clean, extensible starting point. Clone this repo when you want rendering in a new project and build on top of the stable Engine + Application pattern.

## Why This Exists

- Consistent include layout and file organization meant for reuse
- Metal-cpp with no Objective-C in your app code (only a tiny bridge)
- Clear separation of concerns: Application logic vs. Engine rendering
- Easy to extend with new primitives, shaders, and systems

## Highlights

- Component-based rendering: Mesh, Material, Shader, Renderable
- 3D world-space + 2D screen-space (UI) pipelines
- Text rendering via stb_truetype (runtime font atlas)
- EngineIO key-value channel for cross-system data
- Compile-time logging with zero runtime overhead when disabled
- CMake + run.sh workflow; `data/` auto-copied on build

## Getting Started

Prerequisites:
- macOS with Metal support (10.15+ recommended)
- Xcode Command Line Tools
- CMake 3.30+ (or current Homebrew version)

Install CMake if needed:
```bash
brew install cmake
```

Build & run (recommended):
```bash
./run.sh
```

Useful flags:
```bash
./run.sh -b      # build only
./run.sh -v      # verbose configure/build and app logs
EXTRA_CXX_FLAGS="-DENABLE_LOG_DEBUG -DENABLE_LOG_INFO" ./run.sh -v
```

Manual build:
```bash
mkdir -p build && cd build
cmake -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
cp -r ../data .
./application
```

## Architecture & Flow

```
Application → Engine → MeshRenderer → Metal Device/CommandQueue
        ↓           ↓              ↓
Renderables  Camera      Metal Layer/View
```

- `Application` (`src/controller/Application.*`): your entry point. Configure the engine, register world/UI, and implement per-frame logic.
- `Engine` (`src/engine/core/Engine.*`): main loop owner. Creates GLFW window, Metal device/layer, camera, and manages frame lifecycle.
- `MeshRenderer` (`src/engine/systems/MeshRenderer.*`): low-level Metal draw orchestration (command buffers, passes, encoders).

## Folder Layout

```
src/
    main.cpp                      # Metal-cpp private impl defines + app start
    controller/                   # App-level code you customize
        Application.*               # Example app wiring
        ui/ DebugMonitor.*          # Example UI overlay
        world/ WorldDebugMonitor.*  # Example world-space overlay
    engine/
        config.h                    # Global types (Vertex, Mesh, enums)
        backend/glfw/GLFWAdapter.*  # GLFW → Metal layer bridge (Objective-C++)
        components/
            engine/                   # Material, Shader, Renderable
            renderables/              # UI/world containers and primitives
        core/                       # Engine, Camera, FontManager, LogManager, EngineIO
        factories/                  # MeshFactory, PipelineFactory, shape factories
        systems/                    # MeshRenderer and input
        utils/                      # Math, Path, FileReader, Random
data/
    Shaders/ General.metal Text.metal Triangle.metal
    fonts/   Roboto/...
    config.json
```

## Conventions That Matter

- Metal-cpp private implementation macros must appear before any Metal includes in exactly one TU (we use `src/main.cpp`):
    ```cpp
    #define NS_PRIVATE_IMPLEMENTATION
    #define MTL_PRIVATE_IMPLEMENTATION
    #define MTK_PRIVATE_IMPLEMENTATION
    #define CA_PRIVATE_IMPLEMENTATION
    ```
- Shader buffer/sampler layout (follow these indices or things render black):
    - Vertex: `transform` buffer(1), `projection` buffer(2), `view` buffer(3)
    - Fragment: `materialColor` buffer(0), `texture` texture(0), `sampler` sampler(0)
- Include style is anchored at `src/`: use `#include "engine/..."` or `#include "controller/..."` for project headers.
- Always run via `./run.sh` or copy `data/` into `build/` manually.

## Extending the Engine

Add a world-space cube:
```cpp
#include "engine/components/renderables/primitives/3d/WorldCubePrimitive.h"

auto cube = std::make_shared<WorldCubePrimitive>(device, 1.0f, simd::float4{1,0,0,1});
engine->addRenderable(cube->currentRenderable());
```

Add a UI button:
```cpp
#include "engine/components/renderables/primitives/ui/UIButtonPrimitive.h"

TextBoxConfig cfg; /* set paddings, colors */
auto btn = std::make_shared<UIButtonPrimitive>(device, "Click Me", Path::dataPath("fonts/Roboto/Roboto-Light.ttf"), 18.0f, cfg);
btn->setCallback([](){ /* handle click */ });
auto ui = std::make_shared<UIElement>(device);
ui->addPrimitive(btn);
engine->registerUIElement(ui);
```

Custom shader (drop in `data/Shaders/MyShader.metal`) and bind via `Shader("MyShader", device)`; respect buffer indices above.

EngineIO for loose coupling:
```cpp
engine->io().set("renderables.cube.rotation.deg", 45.0f);
auto rot = engine->io().get<float>("renderables.cube.rotation.deg");
```

## Screen-Space vs World-Space

- World-space renderables use camera projection/view.
- UI uses normalized screen coordinates. Use `UIElement` + primitives or create screen quads via `MeshFactory`.

## Logging

Compile-time toggles (zero cost when off):
- `ENABLE_LOG_INFO`, `ENABLE_LOG_DEBUG`, `ENABLE_LOG_ERROR`

Enable with run.sh:
```bash
EXTRA_CXX_FLAGS="-DENABLE_LOG_DEBUG -DENABLE_LOG_INFO" ./run.sh -v
```

## Using This As Your Project Base

1. Clone this repo for a new project
2. Rename the app, adjust window title in `Application.cpp` if desired
3. Add your own controllers under `src/controller/`
4. Add new primitives/shaders under `src/engine/components/renderables/` and `data/Shaders/`
5. Keep includes consistent with `engine/...` and `controller/...`

## Dependencies

- Apple Metal-cpp (included in `deps/Apple/`)
- GLFW (bundled and built from source)
- stb (truetype, image utilities)
- nlohmann/json (header-only)

## License

Use this starter freely in your projects. Attribution appreciated but not required.
