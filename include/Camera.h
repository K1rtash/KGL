#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "ShaderProgram.h"

/**
 * @class Camera
 * @brief Basic 3D camera with perspective view
 * @see Model
 */
class Camera 
{
    float speed = 2.0f, sensitivity = 100.0f;
    const float aspect, width, height; //Logical window resolution
    bool firstClick = true; //Prevents the camera from jumping around when first clicking
    ShaderProgram* shader = nullptr;
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
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
     * @brief Sets the shader program the camera will send its data to
     * @param shader Shader program
     */
    void useShaderProgram(ShaderProgram* shader);

    /**
     * @brief Updates the vertex shader using the camera's matrices
     * @param FOVdeg Sets the camera's FOV using radians
     * @param nearPlane Models nearer than this will be clipped out
     * @param farPlane Models farther than this will be clipped out
     */
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

    /**
     * @brief Sends uniform data to the current shader program
     * @param uniform
     */
    void Matrix(ShaderProgram* shader, const char* uniform);

    /**
     * @brief Handles user-input
     * @param window Window to get input from
     * @param deltaTime Set the input rate independent from the framerate
     */
	void Inputs(GLFWwindow* window, double deltaTime); 
};
#endif