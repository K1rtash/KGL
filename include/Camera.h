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

struct Transform
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
    float speed, sensitivity = 0.3f, sprintSpeed = 2.0f, baseSpeed = 10.0f;
    const float aspect, width, height; //Logical window resolution
    bool firstClick = true; //Prevents the camera from jumping around when first clicking
    Shader* shader = nullptr;
public:
    Transform prev;
    Transform crnt;
    Transform interpolated;
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

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
     * @brief Sends camera data to the current shader program
     *
     * Data sent is the product of the projection and view matrices
     * @param shader Target shader program
     * @param uniform Uniform name
     */
    void Matrix(Shader* shader, const char* uniform);

    /**
     * @brief Handles user-input
     * @param window Window to get input from
     * @note Must be called on the server thread
     */
	void Inputs(GLFWwindow* window, double deltaTime); 
    /**
     * @todo
     */
    void SetAttr(float baseS, float sprintS, float sens) {
        baseSpeed = baseS;
        sprintSpeed = sprintS;
        sensitivity = sens;
    }
};
#endif