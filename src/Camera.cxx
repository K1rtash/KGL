#include "Camera.h"
#include <iostream>

Camera::Camera(float width, float height, glm::vec3 position) : 
    curr{position, glm::quat(1, 0, 0, 0)}, frustum{45.0f, 0.1f, 100.0f, width, height, std::max(0.1f, width / height)}
{
}

void Camera::setViewport(float FOVdeg, float nearPlane, float farPlane)
{
    frustum.nearPlane = nearPlane;
    frustum.farPlane = farPlane;
    frustum.fov = FOVdeg;
}

void Camera::update(double alpha)
{
    intp.pos = prev.pos + (float)alpha * (curr.pos - prev.pos);
    intp.rot = glm::slerp(prev.rot, curr.rot, (float)alpha);

    glm::mat4 rot = glm::mat4_cast(glm::conjugate(intp.rot));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), -intp.pos);
    glm::mat4 proj = glm::perspective(glm::radians(frustum.fov), frustum.ASPECT, frustum.nearPlane, frustum.farPlane);

    cameraMatrix = proj * rot * trans;
}

void Camera::fixedInput(GLFWwindow* window, double deltaTime) 
{    
    prev = curr;

    // ===== ROTACIÓN =====
    float yaw   = -mouseDX * sensitivity;
    float pitch = -mouseDY * sensitivity;
    
    mouseDX = mouseDY = 0.0f;

    glm::quat yawQ   = glm::angleAxis(yaw,   glm::vec3(0,1,0));
    glm::quat pitchQ = glm::angleAxis(pitch, glm::vec3(1,0,0));

    curr.rot = glm::normalize(yawQ * curr.rot * pitchQ);

    // Clamp pitch
    glm::vec3 forward = curr.rot * glm::vec3(0,0,-1);
    float pitchAngle = glm::degrees(asinf(forward.y));
    pitchAngle = glm::clamp(pitchAngle, -89.0f, 89.0f);

    glm::vec3 flatForward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    glm::quat yawOnly = glm::rotation(glm::vec3(0,0,-1), flatForward);
    glm::quat pitchOnly = glm::angleAxis(glm::radians(pitchAngle), glm::vec3(1,0,0));
    curr.rot = yawOnly * pitchOnly;
    
    // ===== MOVIMIENTO =====
    glm::vec3 f = curr.rot * glm::vec3(0,0,-1);
    glm::vec3 r = curr.rot * glm::vec3(1,0,0);
    glm::vec3 u = glm::vec3(0,1,0);
    
    float vel = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W)) curr.pos += f * vel;
    if (glfwGetKey(window, GLFW_KEY_S)) curr.pos -= f * vel;
    if (glfwGetKey(window, GLFW_KEY_A)) curr.pos -= r * vel;
    if (glfwGetKey(window, GLFW_KEY_D)) curr.pos += r * vel;
    if (glfwGetKey(window, GLFW_KEY_SPACE)) curr.pos += u * vel;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) curr.pos -= u * vel;
}

void Camera::captureMouse(GLFWwindow* window)
{    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
        return;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (firstClick)
    {
        glfwSetCursorPos(window, frustum.width / 2.0, frustum.height / 2.0);
        firstClick = false;
        return;
    }

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    mouseDX += float(x - frustum.width  * 0.5f);
    mouseDY += float(y - frustum.height * 0.5f);

    glfwSetCursorPos(window, frustum.width * 0.5, frustum.height * 0.5);
}

void Camera::updateScroll(double scroll_delta)
{
    if (scroll_delta == 0) return;
    frustum.fov -= scroll_delta;
    if(frustum.fov < 1.0f) frustum.fov = 1.0f;
    if(frustum.fov > 120.0f) frustum.fov = 120.0f;
}