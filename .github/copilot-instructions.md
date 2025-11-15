# Metal Renderer Project - AI Coding Agent Guide

## Project Overview
This is a C++20 Metal-based 3D renderer for macOS with a component-based architecture. It uses CMake for builds, GLFW for windowing, and Apple's Metal-cpp C++ bindings for rendering (not Objective-C APIs directly).

## Architecture

### Core Components
- **Engine** (`src/engine/Engine.h`): Main rendering loop orchestrator. Manages window, Metal device/layer, camera controller, and frame lifecycle. Provides `FrameContext` to callbacks with delta time, IO channel, and renderer access.
- **Application** (`src/controller/Application.h`): User-facing entry point. Creates Engine with `EngineConfig`, registers renderables/UI, and implements frame callback logic.
- **MeshRenderer** (`src/systems/MeshRenderer.h`): Low-level Metal rendering system. Manages command buffers, render passes, depth buffers, and draws both 3D renderables and 2D UI elements.

### Renderable System
- **Renderable** (`src/components/engine/Renderable.h`): Wraps a Mesh + Material + transform. Has screen-space vs world-space mode and configurable primitive types.
- **RenderablePrimitive** (`src/components/renderables/primitives/RenderablePrimitive.h`): Base class for higher-level primitives (text, shapes). Manages internal Renderable lifecycle and provides draw methods.
- **Primitives**: `TextPrimitive`, `CirclePrimitive`, `RoundedRectanglePrimitive`, `RectanglePrimitive` in `src/components/renderables/primitives/2d/`.

### Material & Shaders
- **Shader** (`src/components/engine/Shader.h`): Loads `.metal` files from `data/Shaders/`, compiles pipeline state with PipelineFactory.
- **Material** (`src/components/engine/Material.h`): Binds shader, color, texture, and sampler to render pipeline.
- **Shader Convention**: Vertex shaders receive `transform` (buffer 1), `projection` (buffer 2), `view` (buffer 3). Fragment shaders receive `materialColor` (buffer 0). See `data/Shaders/General.metal`.

## Critical Build & Run Workflows

### Standard Build
```bash
./run.sh           # Build (Debug, arm64) + copy data/ to build/ + run
./run.sh -b        # Build only, don't run
./run.sh -v        # Verbose output
```
**Important**: `run.sh` copies `data/` (shaders, fonts, config) into `build/` automatically. Without this, the app can't find runtime assets.

### Manual CMake
```bash
mkdir -p build && cd build
cmake -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
cp -r ../data .    # Don't forget to copy data!
./application
```

### Logging (Compile-Time Controlled)
Logs are **disabled by default** to avoid runtime cost. Enable via preprocessor defines:
```bash
# Enable specific log levels (DEBUG example):
cmake -DCMAKE_CXX_FLAGS="-DENABLE_LOG_DEBUG" ...
```
Available: `ENABLE_LOG_INFO`, `ENABLE_LOG_DEBUG`, `ENABLE_LOG_ERROR`

Use: `LOG_INFO()`, `LOG_DEBUG()`, `LOG_ERROR()`, `LOG_CONSTRUCT()`, `LOG_DESTROY()`, `LOG_START()`, `LOG_FINISH()`, `LOG_STEP()`.

## Project-Specific Conventions

### Metal-cpp Bindings
- Use `Metal/Metal.hpp`, `AppKit/AppKit.hpp`, etc. (C++ wrappers in `deps/Apple/`).
- Main file **must** define private implementation macros before includes:
  ```cpp
  #define NS_PRIVATE_IMPLEMENTATION
  #define MTL_PRIVATE_IMPLEMENTATION
  #define MTK_PRIVATE_IMPLEMENTATION
  #define CA_PRIVATE_IMPLEMENTATION
  ```
- Only `GLFWAdapter.mm` uses Objective-C++ to bridge GLFW window to Metal layer.

### Global Config & Types
- `src/config.h`: Defines `Vertex` struct, `Mesh` struct, includes all Metal/GLFW headers. Include this first in most files.
- Vertex layout: `position` (float3), `color` (float3), `uv` (float2).

### Mesh Creation
- **Factories** (`src/factories/MeshFactory.h`): Procedural mesh builders (`buildCube`, `buildQuad`, `buildScreenQuad`). Return `Mesh` with pre-filled Metal buffers + vertex descriptors.
- **Screen-space quads**: Use `buildScreenQuad(device, left, top, width, height)` for UI in normalized device coordinates [-1, 1].

### Camera & Projection
- Engine provides default camera controller (WASD + mouse look) toggled with `enableDefaultCameraController` in `EngineConfig`.
- Switch projection: `engine->usePerspective()` or `engine->useOrthographic()`.
- Custom views: `engine->setDefaultView(viewMatrix)`.

### IO Channel
- `EngineIO` (`src/engine/EngineIO.h`): Key-value store for passing data between systems. Access via `ctx.io.set()` / `ctx.io.get()`.

### Text Rendering
- Uses `stb_truetype` font atlas + `Text.metal` shader.
- **Font loading**: `FontManager::getInstance().getFont(path, size)` caches fonts.
- `TextPrimitive` rebuilds mesh on text/position changes via `rebuild()` (marks dirty flag).

## Common Pitfalls

1. **Missing `data/` in build**: Shaders/fonts won't load. Always use `./run.sh` or manually copy.
2. **Forgot private implementation defines**: Linker errors for Metal-cpp symbols.
3. **Logs not appearing**: Check if `ENABLE_LOG_*` defined at compile time.
4. **Screen-space primitives off-screen**: Ensure ortho projection matches framebuffer size. Engine auto-sets this, but custom projections may break UI.
5. **Texture/sampler nil crashes**: Shader assumes white (1,1,1,1) if texture sample returns (0,0,0,0). Ensure Material has valid sampler (auto-created if not set).

## Key File Locations
- Entry point: `src/main.cpp` → creates `Application` → calls `run()`.
- Shaders: `data/Shaders/*.metal` (compiled at runtime by Metal).
- Fonts: `data/fonts/` (TTF files for text rendering).
- Dependencies: `deps/glfw`, `deps/json` (nlohmann/json), `deps/stb` (image/font libs).

## Adding New Renderables
1. Subclass `RenderablePrimitive` or create `Renderable` directly.
2. Use `MeshFactory` or build custom `Mesh` (populate `vertexBuffer`, `indexBuffer`, `vertexDescriptor`).
3. Create `Shader` + `Material` (usually `"General"` shader works).
4. Register: `engine->addRenderable(myRenderable)` for 3D or `engine->registerUIElement(myUIContainer)` for UI.

## Testing
No formal test suite. Verify changes by running `./run.sh` and checking visual output + logs (if enabled).
