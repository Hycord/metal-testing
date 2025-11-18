#include "engine/systems/input/InputState.h"

InputState::MouseState InputState::s_mouseState;
InputState::KeyboardState InputState::s_keyboardState;
InputState::WindowState InputState::s_windowState;

void InputState::initialize(float windowWidth, float windowHeight)
{
    s_windowState.width = windowWidth;
    s_windowState.height = windowHeight;
    s_mouseState.x = 0.0;
    s_mouseState.y = 0.0;
    s_mouseState.previousX = 0.0;
    s_mouseState.previousY = 0.0;
    s_mouseState.leftButton = false;
    s_mouseState.rightButton = false;
    s_mouseState.middleButton = false;
    
    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        s_keyboardState.keys[i] = false;
    }
}

void InputState::update(double mouseX, double mouseY, bool leftButton, bool rightButton, bool middleButton)
{
    s_mouseState.previousX = s_mouseState.x;
    s_mouseState.previousY = s_mouseState.y;
    s_mouseState.x = mouseX;
    s_mouseState.y = mouseY;
    s_mouseState.leftButton = leftButton;
    s_mouseState.rightButton = rightButton;
    s_mouseState.middleButton = middleButton;
}

void InputState::updateKeyboard(int key, bool pressed)
{
    if (key >= 0 && key < GLFW_KEY_LAST) {
        s_keyboardState.keys[key] = pressed;
    }
}

void InputState::setWindowSize(float width, float height)
{
    s_windowState.width = width;
    s_windowState.height = height;
}
