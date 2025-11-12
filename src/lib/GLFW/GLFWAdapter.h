#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>

NS::Window* getNSWindow(GLFWwindow* window, CA::MetalLayer* layer);