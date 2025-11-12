#!/bin/bash

# =============================================================================
# Simple CMake Build Script
# =============================================================================

# Config
PROJECT_NAME="application"
BUILD_DIR="build"
DATA_DIR="data"
EXECUTABLE_NAME="${PROJECT_NAME}"
VERBOSE=false

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# =============================================================================
# Utils
# =============================================================================

log() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
    exit 1
}

# Check tools
check_tools() {
    command -v cmake >/dev/null 2>&1 || error "cmake required"
    command -v make >/dev/null 2>&1 || command -v ninja >/dev/null 2>&1 || error "make or ninja required"
}

# CPU count
cpu_count() {
    nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1
}

# =============================================================================
# Build
# =============================================================================

clean() {
    rm -rf "$BUILD_DIR"
}

setup() {
    mkdir -p "$BUILD_DIR" || error "Cannot create $BUILD_DIR"
    [ -d "$DATA_DIR" ] && {
        rm -rf "$BUILD_DIR/$DATA_DIR" 2>/dev/null
        cp -r "$DATA_DIR" "$BUILD_DIR/" 2>/dev/null
        $VERBOSE && log "Copied data"
    }
}

configure() {
    cd "$BUILD_DIR" || error "Cannot enter $BUILD_DIR"
    if $VERBOSE; then
        if ! cmake -DCMAKE_BUILD_TYPE=Debug ..; then
            cd ..
            error "CMake config failed"
        fi
    else
        if ! cmake -DCMAKE_BUILD_TYPE=Debug .. >/dev/null 2>&1; then
            cd ..
            error "CMake config failed"
        fi
    fi
    cd ..
    $VERBOSE && log "Configured"
}

build() {
    cd "$BUILD_DIR" || error "Cannot enter $BUILD_DIR"
    local jobs="-j$(cpu_count)"
    
    if command -v ninja >/dev/null 2>&1; then
        if $VERBOSE; then
            ninja $jobs || error "Ninja build failed"
        else
            ninja $jobs >/dev/null 2>&1 || error "Ninja build failed"
        fi
    else
        if $VERBOSE; then
            make $jobs || error "Make build failed"
        else
            make $jobs -s >/dev/null 2>&1 || error "Make build failed"
        fi
    fi
    cd ..
}

# =============================================================================
# Run
# =============================================================================

run() {
    local exec_path="$BUILD_DIR/$EXECUTABLE_NAME"
    [ -f "$exec_path" ] || error "Executable not found: $exec_path"
    chmod +x "$exec_path" 2>/dev/null
    exec "$exec_path" "$@"
}

# =============================================================================
# Main
# =============================================================================

main() {
    [ -f "CMakeLists.txt" ] || error "Run from project root (needs CMakeLists.txt)"
    
    local build_only=false
    local args=()
    
    while [ $# -gt 0 ]; do
        case "$1" in
            -v|--verbose) VERBOSE=true; shift ;;
            -b|--build) build_only=true; shift ;;
            -h|--help) show_help; exit 0 ;;
            *) args+=("$1"); shift ;;
        esac
    done
    
    check_tools
    
    log "Starting build"
    clean
    setup
    configure
    build
    log "Build complete"
    
    [ $build_only = false ] && run "${args[@]}"
}

show_help() {
    cat << EOF
Usage: $0 [-b] [-v] [args...]

Options:
  -b, --build    Build only (don't run)
  -v, --verbose  Show all build steps
  -h, --help     This help

Builds every time, copies data, runs with arguments.
EOF
}

# Run
trap 'error "Interrupted"' INT
main "$@"
