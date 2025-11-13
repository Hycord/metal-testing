# Metal Renderer Template

A small, minimal template C++ renderer project configured for Apple Metal on macOS.

Use this repository as a starting point for future Metal-based projects. It contains a minimal CMake-based build, example shader and source layout, and a small runtime wrapper to get a Metal rendering loop up and running quickly.

## Goals

- Provide a lightweight, reusable starting point for Metal experiments and demos.
- Keep the build simple (CMake) and macOS-friendly (Xcode/CMake workflows supported).
- Include a clear project layout and basic examples for shaders, rendering, and view/controller glue.

## Features

- Cross-compile with CMake for command-line build or Xcode.
- Small sample renderer with Metal shaders located in `data/Shaders/`.
- Simple file layout in `src/` for view, controller, math, and utilities.

## Prerequisites

- macOS (recent, supporting Metal)
- Xcode (for Metal SDK & toolchain)
- Command line tools (clang, make)
- CMake (>= 3.15 recommended)

Install common tools via Homebrew (optional):

```bash
# install cmake if you don't have it
brew install cmake
```

## Build (command-line)

There are two recommended ways to build this template: the included `run.sh` helper (recommended) or a manual CMake workflow.

1) Quick (recommended): use the `run.sh` helper script

```bash
# Build & run (default: Debug, copies `data/` into `build/`)
./run.sh

# Build only (don't run):
./run.sh -b

# Show verbose output during configure & build:
./run.sh -v
```

`run.sh` will configure CMake (sets a macOS architecture and Debug build type by default), copy `data/` into `build/`, build with `make` or `ninja` (if available), and then run the produced `application` binary.

2) Manual CMake (more control)

From the repository root do a separate configure and build step. This example sets the macOS architecture and a Release build:

```bash
mkdir -p build && cd build
# configure (set architecture and build type explicitly)
cmake -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_BUILD_TYPE=Release ..

# build (uses CMake's --build wrapper; pass extra args to the underlying builder)
cmake --build . --config Release -- -j$(sysctl -n hw.ncpu)
```

This will produce the `application` executable in `build/` (see the generated `Makefile` and CMake cache in `build/`).

To generate an Xcode project instead of a Makefile/ninja build:

```bash
cmake -S . -B build -G Xcode
open build
```

## Run

There is a small helper script `run.sh` in the repository root that runs the built application. From the repo root:

```bash
./run.sh
```

Or run the executable directly from `build/application`.

## Project layout

- `CMakeLists.txt` — top-level CMake configuration
- `run.sh` — small runner script
- `data/` — runtime assets and Metal shaders
  - `Shaders/` — Metal shader files (`*.metal`)
- `deps/` — third-party headers and helper include files
- `src/` — application source
  - `main.cpp` — platform/application entry
  - `lib/` — modules: `FileReader`, `LogManager`, `MetalController`, `MetalView`, `MetalMath` etc.

Example important files in `src/`:
- `src/main.cpp` — bootstraps the application and Metal view
- `src/lib/MetalController/` — app & view delegate glue
- `src/lib/MetalView/Renderer.cpp` — renderer implementation
