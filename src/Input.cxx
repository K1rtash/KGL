#include "Input.h"
#include <iostream>

KGL_KeyState keyState[GLFW_KEY_LAST] = { KGL_KeyState::Up };
bool keyboard[GLFW_KEY_LAST] = { false };

void updateKeyboard(GLFWwindow* window)
{
    for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; i++)
    {
        bool isDown = keyboard[i];

        switch (keyState[i])
        {
            case KGL_KeyState::Up:
                if(isDown) keyState[i] = KGL_KeyState::Press;
                break;
            case KGL_KeyState::Press:
                keyState[i] = isDown ? KGL_KeyState::Hold : KGL_KeyState::Release;
                break;
            case KGL_KeyState::Hold:
                if(!isDown) keyState[i] = KGL_KeyState::Release;
                break;
            case KGL_KeyState::Release:
                keyState[i] = isDown ? KGL_KeyState::Press : KGL_KeyState::Up;
                break;
        }     
    }
}

KGL_KeyState getKey(int key)
{
    return keyState[key];
}