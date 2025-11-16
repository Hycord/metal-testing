# Metal Renderer Template

A C++20 Metal-based 3D renderer template for macOS with a component-based architecture. Use this as a starting point for building applications that need Metal rendering capabilities.

## Overview

This template provides a complete rendering framework with:
- **Component-based architecture**: Renderable primitives, materials, shaders, and meshes
- **Metal-cpp bindings**: Modern C++ wrapper for Metal API (no Objective-C required)
- **Built-in primitives**: Text, circles, rectangles, rounded rectangles, and custom meshes
- **Camera system**: WASD + mouse look controller with perspective/orthographic projection
- **Asset pipeline**: Automatic shader compilation, font loading, and texture management
- **CMake build system**: Simple configuration for macOS arm64/x86_64

## Key Features

- **Engine + Application pattern**: Separation of rendering loop from application logic
- **Flexible rendering**: Both 3D world-space and 2D screen-space rendering
- **Text rendering**: Built-in support via stb_truetype with font atlas generation
- **Material system**: Shader + color + texture + sampler binding
- **Procedural mesh generation**: Factories for common shapes (cubes, quads, screen-space UI)
- **IO Channel**: Key-value store for inter-system communication
- **Compile-time logging**: Zero-cost abstractions when disabled

## Prerequisites

- macOS (supporting Metal - macOS 10.15+)
- Xcode Command Line Tools (for Metal SDK & clang toolchain)
- CMake (>= 3.15)

Install CMake via Homebrew if needed:

```bash
brew install cmake
```

## Quick Start

The fastest way to get started is with the included `run.sh` script:

```bash
# Build & run (Debug mode, arm64)
./run.sh

# Build only (skip running)
./run.sh -b

# Verbose output
./run.sh -v
```

This automatically:
1. Configures CMake for your architecture
2. Copies `data/` (shaders, fonts, config) into `build/`
3. Builds the application
4. Runs the executable

**Important**: Always copy `data/` to `build/` or the app won't find runtime assets!

## Build (command-line)

### Option 1: Using run.sh (Recommended)

```bash
# Build & run (default: Debug, copies `data/` into `build/`)
./run.sh

# Build only (don't run):
./run.sh -b

# Show verbose output during configure & build:
./run.sh -v
```

### Option 2: Manual CMake

```bash
mkdir -p build && cd build

# Configure for arm64 Debug build
cmake -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_BUILD_TYPE=Debug ..

# Build with parallel jobs
cmake --build . -- -j$(sysctl -n hw.ncpu)

# Don't forget to copy runtime assets!
cp -r ../data .

# Run
./application
```

### Option 3: Xcode Project

```bash
cmake -S . -B build -G Xcode
open build/renderer.xcodeproj
```

## Logging (compile-time)

Logging is **disabled by default** to avoid runtime cost. Enable specific log levels via preprocessor definitions:

### Available Log Levels

- `ENABLE_LOG_INFO` - General information messages
- `ENABLE_LOG_DEBUG` - Detailed debugging output
- `ENABLE_LOG_ERROR` - Error messages
- Plus: `LOG_CONSTRUCT()`, `LOG_DESTROY()`, `LOG_START()`, `LOG_FINISH()`, `LOG_STEP()`

### Enable Logging

Using CMake:

```bash
mkdir -p build && cd build
cmake -DCMAKE_CXX_FLAGS="-DENABLE_LOG_DEBUG" ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

Using run.sh:

```bash
EXTRA_CXX_FLAGS="-DENABLE_LOG_DEBUG -DENABLE_LOG_INFO" ./run.sh -v
```

**Note**: When disabled at compile-time, log calls are completely removed (zero runtime cost).
## Architecture

### Core Components

```
Application → Engine → MeshRenderer
    ↓           ↓           ↓
Renderables  Camera   Metal Device/Layer
```

- **Engine** (`src/engine/Engine.h`): Main rendering loop orchestrator. Manages window, Metal device/layer, camera, and frame lifecycle.
- **Application** (`src/controller/Application.h`): User-facing entry point. Create your custom application logic here.
- **MeshRenderer** (`src/systems/MeshRenderer.h`): Low-level Metal rendering system. Handles command buffers, render passes, and drawing.

### Renderable System

- **Renderable** (`src/components/engine/Renderable.h`): Wraps a Mesh + Material + transform matrix
- **RenderablePrimitive**: Base class for higher-level primitives (text, shapes)
- **Built-in Primitives**: 
  - `TextPrimitive` - TrueType font rendering with atlas
  - `CirclePrimitive`, `RectanglePrimitive`, `RoundedRectanglePrimitive` - 2D shapes
  - Custom meshes via `MeshFactory`

### Material & Shader System

- **Shader** (`src/components/engine/Shader.h`): Loads `.metal` files from `data/Shaders/`, compiles pipeline state
- **Material** (`src/components/engine/Material.h`): Binds shader, color, texture, and sampler
- **Shader Convention**: 
  - Vertex: `transform` (buffer 1), `projection` (buffer 2), `view` (buffer 3)
  - Fragment: `materialColor` (buffer 0)

## Project Layout

```
renderer/
├── CMakeLists.txt          # Build configuration
├── run.sh                  # Build & run helper script
├── data/                   # Runtime assets (copied to build/)
│   ├── Shaders/            # Metal shader files (*.metal)
│   │   ├── General.metal   # Standard 3D shader
│   │   ├── Text.metal      # Text rendering shader
│   │   └── Triangle.metal  # Example primitive shader
│   ├── fonts/              # TrueType fonts for text rendering
│   └── config.json         # Application configuration
├── deps/                   # Third-party dependencies
│   ├── Apple/              # Metal-cpp C++ bindings
│   ├── glfw/               # Windowing library
│   ├── json/               # nlohmann/json
│   └── stb/                # stb_truetype, stb_image
└── src/                    # Application source code
    ├── main.cpp            # Entry point
    ├── config.h            # Global types (Vertex, Mesh)
    ├── engine/             # Engine core (Engine, EngineIO)
    ├── controller/         # Application logic
    ├── systems/            # MeshRenderer, PipelineFactory
    ├── components/         # Renderables, Materials, Shaders
    │   ├── engine/         # Core components
    │   └── renderables/    # Primitive implementations
    ├── factories/          # MeshFactory, FontManager
    ├── backend/            # GLFW adapter (GLFWAdapter.mm)
    └── utils/              # Logging, file utilities
```

## Using This Template

### 1. Create a Custom Application

Edit `src/controller/Application.cpp` or create your own:

```cpp
#include "engine/Engine.h"

class MyApp {
public:
    void setup(Engine* engine) {
        // Register renderables, set up scene
        auto cube = std::make_shared<Renderable>(
            MeshFactory::buildCube(engine->getDevice(), 2.0f),
            std::make_shared<Material>(
                std::make_shared<Shader>("General", engine->getDevice()),
                simd::make_float4(1, 0, 0, 1)
            )
        );
        engine->addRenderable(cube);
    }
    
    void onFrame(const FrameContext& ctx) {
        // Per-frame logic
    }
};
```

### 2. Add Custom Shaders

Create `data/Shaders/MyShader.metal`:

```metal
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
};

vertex float4 vertex_main(
    VertexIn in [[stage_in]],
    constant float4x4& transform [[buffer(1)]],
    constant float4x4& projection [[buffer(2)]],
    constant float4x4& view [[buffer(3)]]
) {
    return projection * view * transform * float4(in.position, 1.0);
}

fragment float4 fragment_main(constant float4& color [[buffer(0)]]) {
    return color;
}
```

### 3. Create Custom Renderables

Subclass `RenderablePrimitive` or use `Renderable` directly:

```cpp
auto mesh = MeshFactory::buildQuad(device, width, height);
auto shader = std::make_shared<Shader>("MyShader", device);
auto material = std::make_shared<Material>(shader, float4{1, 1, 1, 1});
auto renderable = std::make_shared<Renderable>(mesh, material);
```

### 4. Text Rendering

```cpp
auto font = FontManager::getInstance().getFont("data/fonts/Roboto/Roboto-Regular.ttf", 48);
auto text = std::make_shared<TextPrimitive>(
    "Hello World", 
    device, 
    font, 
    simd::make_float3(100, 100, 0)  // Screen position
);
engine->registerUIElement(text);
```

## Common Pitfalls

1. **Missing data/ folder**: Shaders/fonts won't load. Always use `./run.sh` or manually copy `data/` to `build/`
2. **Private implementation defines**: If you get linker errors, ensure your main file has these before Metal includes:
   ```cpp
   #define NS_PRIVATE_IMPLEMENTATION
   #define MTL_PRIVATE_IMPLEMENTATION
   #define MTK_PRIVATE_IMPLEMENTATION
   #define CA_PRIVATE_IMPLEMENTATION
   ```
3. **Screen-space coordinates**: Use `MeshFactory::buildScreenQuad()` for UI elements in normalized device coordinates [-1, 1]
4. **Camera projection**: Switch with `engine->usePerspective()` or `engine->useOrthographic()`

## Dependencies

- **Metal-cpp**: C++ bindings for Metal API (included in `deps/Apple/`)
- **GLFW**: Cross-platform windowing and input (submodule)
- **stb_truetype**: Font loading and rendering
- **nlohmann/json**: JSON configuration parsing

## License

This template is provided as-is for use in your own projects. Modify and extend as needed.
