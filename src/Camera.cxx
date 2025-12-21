#include "Camera.h"
#include "Input.h"

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

void Camera::updateMatrix(double alpha)
{
    intp.pos = prev.pos + (float)alpha * (curr.pos - prev.pos);
    intp.rot = glm::slerp(prev.rot, curr.rot, (float)alpha);

    glm::mat4 view = glm::mat4_cast(glm::conjugate(intp.rot));
    view = glm::translate(view, -intp.pos);
    glm::mat4 proj = glm::perspective(glm::radians(frustum.fov), frustum.aspect, frustum.nearPlane, frustum.farPlane);

    cameraMatrix = proj * view;
}

void Camera::updateFixedInput(GLFWwindow* window, double deltaTime) 
{    
    prev = curr;
    bool key_W = (getKey(GLFW_KEY_W) == KGL_KeyState::Press || getKey(GLFW_KEY_W) == KGL_KeyState::Hold);
    bool key_A = (getKey(GLFW_KEY_A) == KGL_KeyState::Press || getKey(GLFW_KEY_A) == KGL_KeyState::Hold);
    bool key_S = (getKey(GLFW_KEY_S) == KGL_KeyState::Press || getKey(GLFW_KEY_S) == KGL_KeyState::Hold);
    bool key_D = (getKey(GLFW_KEY_D) == KGL_KeyState::Press || getKey(GLFW_KEY_D) == KGL_KeyState::Hold);
    bool key_SPACE = (getKey(GLFW_KEY_SPACE) == KGL_KeyState::Press || getKey(GLFW_KEY_SPACE) == KGL_KeyState::Hold);
    bool key_LCTRL = (getKey(GLFW_KEY_LEFT_CONTROL) == KGL_KeyState::Press || getKey(GLFW_KEY_LEFT_CONTROL) == KGL_KeyState::Hold);
    bool key_LSHIFT = (getKey(GLFW_KEY_LEFT_SHIFT) == KGL_KeyState::Press || getKey(GLFW_KEY_LEFT_SHIFT) == KGL_KeyState::Hold);

    // ===== ROTACIÓN =====
    float dx = -mouseDX * sensitivity; // aquí asume que sensitivity está en grados por píxel
    float dy = -mouseDY * sensitivity;

    mouseDX = mouseDY = 0.0f;

    // limita el delta por frame para evitar saltos bestias
    float maxDelta = 5.0f;
    dx = glm::clamp(dx, -maxDelta, maxDelta);
    dy = glm::clamp(dy, -maxDelta, maxDelta);

    yaw   += dx;
    pitch += dy;

    // clamp del pitch
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // construir el quaternion a partir de yaw/pitch
    glm::quat qYaw   = glm::angleAxis(glm::radians(yaw),   glm::vec3(0,1,0));
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1,0,0));

    curr.rot = glm::normalize(qYaw * qPitch);

    // ===== MOVIMIENTO =====
    glm::vec3 VecForward = curr.rot * glm::vec3(0,0,-1);
    glm::vec3 VecRight = curr.rot * glm::vec3(1,0,0);
    glm::vec3 VecUp = glm::vec3(0,1,0);

    float speed0 = speed;
    if(key_LSHIFT) speed0 *= 2.0f;
    
    float vel = speed0 * (float)deltaTime;


    if (key_W) curr.pos += VecForward * vel;
    if (key_S) curr.pos -= VecForward * vel;
    if (key_A) curr.pos -= VecRight * vel;
    if (key_D) curr.pos += VecRight * vel;
    if (key_SPACE) curr.pos += VecUp * vel;
    if (key_LCTRL) curr.pos -= VecUp * vel;
}

/*void Camera::captureMouse(GLFWwindow* window)
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
}*/

void Camera::updateScroll(double scroll_delta)
{
    if (scroll_delta == 0) return;
    frustum.fov -= scroll_delta;
    if(frustum.fov < 1.0f) frustum.fov = 1.0f;
    if(frustum.fov > 120.0f) frustum.fov = 120.0f;
}

void Camera::updateCursor(float dx, float dy) 
{
    mouseDX += dx;
    mouseDY += dy;
}

const Camera::Transform* Camera::getTransform() const
{ 
    return &intp; 
}

const glm::mat4* Camera::getMatrix() const
{
    return &cameraMatrix;
}