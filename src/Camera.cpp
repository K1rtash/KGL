#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position) : width{width}, height{height}, Position{position} 
{
    Aspect(width, height);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram& shader, const char* uniform)
{
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(Position, Position + Orientation, Up);
    projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);
    
    // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window) 
{
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += speed * Orientation;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position += speed * -glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position += speed * -Orientation;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += speed * glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) Position += speed * Up;
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) Position += speed * -Up;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = 0.4f;
    else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) speed = 0.1f; 
    
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

void Camera::Aspect(int width, int height) 
{
    this->width = width;
    this->height = height;
    aspect = static_cast<float>(width) / static_cast<float>(height);
}
