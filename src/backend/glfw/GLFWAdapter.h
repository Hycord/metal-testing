#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <AppKit/AppKit.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

NS::Window* get_ns_window(GLFWwindow* glfwWindow, CA::MetalLayer* layer);
void set_display_sync_enabled(CA::MetalLayer* layer, bool enabled);
