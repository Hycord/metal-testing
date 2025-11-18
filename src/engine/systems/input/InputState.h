#pragma once

#include <GLFW/glfw3.h>

/**
 * Global input and application state accessible from any Renderable.
 * This is a singleton-style static manager for mouse, keyboard, and window state.
 */
class InputState {
public:
    struct MouseState {
        double x = 0.0;
        double y = 0.0;
        double previousX = 0.0;
        double previousY = 0.0;
        bool leftButton = false;
        bool rightButton = false;
        bool middleButton = false;
    };

    struct KeyboardState {
        bool keys[GLFW_KEY_LAST] = {};
    };

    struct WindowState {
        float width = 800.0f;
        float height = 600.0f;
    };

    static void initialize(float windowWidth, float windowHeight);

    static void update(double mouseX, double mouseY, bool leftButton, bool rightButton, bool middleButton);

    static void updateKeyboard(int key, bool pressed);

    static void setWindowSize(float width, float height);

    static const MouseState& getMouseState() { return s_mouseState; }
    static const KeyboardState& getKeyboardState() { return s_keyboardState; }
    static const WindowState& getWindowState() { return s_windowState; }

    static double getMouseX() { return s_mouseState.x; }
    static double getMouseY() { return s_mouseState.y; }
    static double getMouseDeltaX() { return s_mouseState.x - s_mouseState.previousX; }
    static double getMouseDeltaY() { return s_mouseState.y - s_mouseState.previousY; }
    static bool isKeyPressed(int key) { return s_keyboardState.keys[key]; }
    static float getWindowWidth() { return s_windowState.width; }
    static float getWindowHeight() { return s_windowState.height; }

private:
    static MouseState s_mouseState;
    static KeyboardState s_keyboardState;
    static WindowState s_windowState;
};
