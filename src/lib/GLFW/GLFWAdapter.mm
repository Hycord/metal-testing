#import "GLFWAdapter.h"

NS::Window* getNSWindow(GLFWwindow* window, CA::MetalLayer* layer){
    CALayer* objectLayer = (__bridge CALayer*) layer;
    NSWindow* objectWindow = glfwGetCocoaWindow(window);

    objectWindow.contentView.layer = objectLayer;
    objectWindow.contentView.wantsLayer = YES;

    return (__bridge NS::Window*) objectWindow;
}
