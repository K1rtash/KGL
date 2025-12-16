#include "Camera.h"
#include <iostream>

Camera::Camera(float width, float height, glm::vec3 position) : 
    width{width}, height{height}, aspect{std::max(0.1f, width / height)}, crnt{position, glm::quat(1, 0, 0, 0)}
{
    setViewport(45.0f, 0.1f, 100.0f);
}

void Camera::setViewport(float FOVdeg, float nearPlane, float farPlane)
{
    projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);
}

void Camera::update(double alpha)
{
    glm::mat4 view = glm::mat4(1.0f); // Initializes matrices since otherwise they will be the null matrix

    interpolated.pos = prev.pos + (float)alpha * (crnt.pos - prev.pos);
    interpolated.rot = glm::slerp(prev.rot, crnt.rot, (float)alpha);

    glm::mat4 rotation = glm::mat4_cast(glm::conjugate(interpolated.rot));
    glm::mat4 transformation = glm::translate(glm::mat4(1.0f), -interpolated.pos);
    
    cameraMatrix = projection * (rotation * transformation);
}

void Camera::Matrix(Shader* shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, double deltaTime) 
{
    // Vectores locales
    glm::vec3 Up = glm::vec3(0, 1, 0);
    glm::vec3 Forward = crnt.rot * glm::vec3(0, 0, -1);
    glm::vec3 Right   = crnt.rot * glm::vec3(1, 0, 0);

    // Movimiento
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = baseSpeed * sprintSpeed;
    else speed = baseSpeed;

    float velocity = speed * (float)deltaTime;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) crnt.pos += Forward * velocity;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) crnt.pos -= Forward * velocity;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) crnt.pos += Right * velocity;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) crnt.pos -= Right * velocity;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) crnt.pos += Up * velocity;
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) crnt.pos -= Up * velocity;

    // Rotación con ratón
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if(firstClick)
        {
            glfwSetCursorPos(window, width * 0.5, height * 0.5);
            firstClick = false;
            return;
        }

        // Obtén delta del ratón
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float dx = (float)(mouseX - width * 0.5);
        float dy = (float)(mouseY - height * 0.5);

        // Delta de rotación suavizado con deltaTime
        float rotX = -dy * sensitivity * deltaTime;
        float rotY = -dx * sensitivity * deltaTime;

        glm::quat pitchDelta = glm::angleAxis(rotX, glm::vec3(1,0,0));
        glm::quat yawDelta   = glm::angleAxis(rotY, glm::vec3(0,1,0));

        // Aplica delta directamente
        crnt.rot = glm::normalize(yawDelta * crnt.rot * pitchDelta);

        // Limita pitch para evitar voltear la cámara
        glm::vec3 forward = crnt.rot * glm::vec3(0,0,-1);
        float pitch = glm::degrees(asin(forward.y));
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Reconstruye solo pitch
        glm::vec3 yawForward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
        glm::quat yawQuat = glm::rotation(glm::vec3(0,0,-1), yawForward);
        glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), glm::vec3(1,0,0));
        crnt.rot = yawQuat * pitchQuat;

        // Re-centra el cursor
        glfwSetCursorPos(window, width * 0.5, height * 0.5);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}