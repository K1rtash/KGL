#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

enum class KGL_KeyState : unsigned int {Up, Press, Hold, Release};

extern KGL_KeyState keyState[GLFW_KEY_LAST];
extern bool keyboard[GLFW_KEY_LAST];    

void updateKeyboard(GLFWwindow* window);
KGL_KeyState getKey(int key);

#endif