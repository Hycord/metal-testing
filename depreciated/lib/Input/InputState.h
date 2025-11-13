#pragma once

#include <GLFW/glfw3.h>

/**
 * Global input and application state accessible from any Renderable.
 * This is a singleton-style static manager for mouse, keyboard, and window state.
 */
class InputState {
public:
    // Mouse state
    struct MouseState {
        double x = 0.0;
        double y = 0.0;
        double prevX = 0.0;
        double prevY = 0.0;
        bool leftButton = false;
        bool rightButton = false;
        bool middleButton = false;
    };

    // Keyboard state (track key states)
    struct KeyboardState {
        bool keys[GLFW_KEY_LAST] = {};
    };

    // Window state
    struct WindowState {
        float width = 800.0f;
        float height = 600.0f;
    };

    // Initialize the input state (should be called once at startup)
    static void initialize(float windowWidth, float windowHeight);

    // Update input state (should be called once per frame)
    static void update(double mouseX, double mouseY, bool leftButton, bool rightButton, bool middleButton);

    // Update keyboard state
    static void updateKeyboard(int key, bool pressed);

    // Update window size
    static void setWindowSize(float width, float height);

    // Getters for static access
    static const MouseState& getMouseState() { return s_mouseState; }
    static const KeyboardState& getKeyboardState() { return s_keyboardState; }
    static const WindowState& getWindowState() { return s_windowState; }

    // Convenience getters
    static double getMouseX() { return s_mouseState.x; }
    static double getMouseY() { return s_mouseState.y; }
    static double getMouseDeltaX() { return s_mouseState.x - s_mouseState.prevX; }
    static double getMouseDeltaY() { return s_mouseState.y - s_mouseState.prevY; }
    static bool isKeyPressed(int key) { return s_keyboardState.keys[key]; }
    static float getWindowWidth() { return s_windowState.width; }
    static float getWindowHeight() { return s_windowState.height; }

private:
    static MouseState s_mouseState;
    static KeyboardState s_keyboardState;
    static WindowState s_windowState;
};
