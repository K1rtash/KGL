#include "Camera.h"
#include <iostream>

Camera::Camera(float width, float height, glm::vec3 position) : 
    width{width}, height{height}, aspect{std::max(0.1f, width / height)}, Position{position}
{
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(Position, Position + Orientation, Up);
    projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);
    
    cameraMatrix = projection * view;
}

void Camera::useShaderProgram(ShaderProgram* shader) {
    if (shader != nullptr) this->shader = shader; else std::cout << "camera provided shader program is invalid, cant switch" << std::endl;
}

void Camera::Matrix(ShaderProgram* shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, double deltaTime) 
{
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = 4.0f;
    else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) speed = 2.0f; 

    float velocity = speed * (float)deltaTime; 

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += velocity * Orientation;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position += velocity * -glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position += velocity * -Orientation;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += velocity * glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) Position += velocity * Up;
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) Position += velocity * -Up;
    
    
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if(firstClick) {
            glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) Orientation = newOrientation;

        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}