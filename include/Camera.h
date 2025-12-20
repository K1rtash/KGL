#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "Shader.h"

struct CameraTransform
{
    glm::vec3 pos;
    glm::quat rot;
};

/**
 * @class Camera
 * @brief Basic 3D camera with perspective view
 * @see Mesh
 */
class Camera 
{
    bool firstClick = true;
    CameraTransform prev;
    CameraTransform curr;
    CameraTransform intp;

    struct Frustum
    {
        float fov, nearPlane, farPlane;
        float width, height;
        const float ASPECT;
    } frustum;
public:
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    float mouseDX = 0.0f, mouseDY = 0.0f;

    float speed = 5.0f;
    float sensitivity = 0.0025f;

    /**
     * @brief Creates the object
     * @param width Logical window width
     * @param height Logical window height
     * @param position World coordinates to summon the camera at
     */
    Camera(float width, float height, glm::vec3 position);

    /**
     * @brief Updates the vertex shader using the camera's matrices
     * @param alpha Interpolation
     * @note Must be called only on the render thread
     */
    void update(double alpha);
    
    /**
     * @brief Defines the camera viewport
     * @param FOVdeg Sets the camera's FOV using radians
     * @param nearPlane Models nearer than this will be clipped out
     * @param farPlane Models farther than this will be clipped out
     */
    void setViewport(float FOVdeg, float nearPlane, float farPlane);

    /**
     * @brief Handles event-like user-input like keys
     * @param window Window to get input from
     * @note Must be called on the logic thread
     */
	void fixedInput(GLFWwindow* window, double deltaTime); 
    /**
     * @todo
     */
    void captureMouse(GLFWwindow* window);
    /**
     * @todo
     */
    const CameraTransform* getTransform() const { return &intp; } 
    /**
     * 
     */
    void updateScroll(double scroll_delta);
};
#endif