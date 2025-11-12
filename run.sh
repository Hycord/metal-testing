#!/bin/bash

# =============================================================================
# Minimal CMake Build Script
# =============================================================================

# Config
PROJECT_NAME="application"
BUILD_DIR="build"
SRC_DIR="src"
LIB_DIR="lib"
EXTERNAL_DIR="deps"
DATA_DIR="data"
EXECUTABLE_NAME="${PROJECT_NAME}"
HASH_FILE="${BUILD_DIR}/.build_hash"
DEFAULT_RUN=true
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
    if $VERBOSE; then
        echo -e "${GREEN}[INFO]${NC} $1"
    fi
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
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

# Dir hash (fixed)
dir_hash() {
    local dir="$1"
    [ -d "$dir" ] || { echo "0"; return; }
    
    # Create temp file for consistent hashing
    local temp_file
    temp_file=$(mktemp)
    
    # Get sorted list of files and their paths
    if ! find "$dir" -type f -not -path "*/.*" | sort | while read -r file; do
        relative="${file#$dir/}"
        echo "--- $relative ---" >> "$temp_file"
        cat "$file" >> "$temp_file" 2>/dev/null || echo "read_error" >> "$temp_file"
        echo "" >> "$temp_file"
    done; then
        warn "Hashing error in $dir"
        rm -f "$temp_file"
        echo "0"
        return
    fi
    
    # Calculate hash
    local hash
    if command -v sha256sum >/dev/null 2>&1; then
        hash=$(sha256sum "$temp_file" 2>/dev/null | cut -d' ' -f1)
    elif command -v shasum >/dev/null 2>&1; then
        hash=$(shasum -a 256 "$temp_file" 2>/dev/null | cut -d' ' -f1)
    else
        hash="0"
    fi
    
    rm -f "$temp_file"
    echo "${hash:-0}"
}

# Needs rebuild?
needs_rebuild() {
    local src_hash=$(dir_hash "$SRC_DIR")
    local lib_hash=$(dir_hash "$LIB_DIR")
    local ext_hash=$(dir_hash "$EXTERNAL_DIR")
    
    local current_hash="${src_hash}_${lib_hash}_${ext_hash}"
    
    if [ ! -d "$BUILD_DIR" ] || [ ! -f "$HASH_FILE" ]; then
        return 0  # Needs rebuild
    fi
    
    local prev_hash
    prev_hash=$(cat "$HASH_FILE" 2>/dev/null || echo "")
    
    [ "$current_hash" != "$prev_hash" ] && return 0
    return 1  # No rebuild needed
}

update_hash() {
    local src_hash=$(dir_hash "$SRC_DIR")
    local lib_hash=$(dir_hash "$LIB_DIR")
    local ext_hash=$(dir_hash "$EXTERNAL_DIR")
    mkdir -p "$BUILD_DIR"
    echo "${src_hash}_${lib_hash}_${ext_hash}" > "$HASH_FILE"
}

# =============================================================================
# Build
# =============================================================================

clean() {
    rm -rf "$BUILD_DIR"
    log "Cleaned build dir"
}

setup() {
    mkdir -p "$BUILD_DIR" || error "Cannot create $BUILD_DIR"
    [ -d "$DATA_DIR" ] && cp -r "$DATA_DIR" "$BUILD_DIR/" 2>/dev/null && log "Copied data"
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
    log "Configured"
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
    log "Built"
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
    
    local args=()
    while [ $# -gt 0 ]; do
        case "$1" in
            -v|--verbose) VERBOSE=true; shift ;;
            -c|--clean) clean; exit 0 ;;
            -b|--build) shift; DEFAULT_RUN=false; break ;;
            -h|--help) show_help; exit 0 ;;
            *) args+=("$1"); shift ;;
        esac
    done
    
    check_tools
    
    if ! needs_rebuild; then
        log "Build up to date"
    else
        clean
        setup
        configure
        build
        update_hash
    fi
    
    $DEFAULT_RUN && run "${args[@]}"
}

show_help() {
    cat << EOF
Usage: $0 [run|build] [args...]

Options:
  -v, --verbose  Show build steps
  -c, --clean    Clean build dir
  -h, --help     This help
  -b, --build    Only build without running the project

Default: build and run
EOF
}

# Run
trap 'error "Interrupted"' INT
main "$@"
