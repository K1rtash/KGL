#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include "Shader.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

/**
 * @class Camera
 * @brief Basic 3D camera with perspective view
 * @see Mesh
 */
class Camera {
private:
    struct Frustum {
        float fov, nearPlane, farPlane;
        float width, height;
        float aspect;
    }; 
    struct Transform {
        glm::vec3 pos;
        glm::quat rot;
    };
public:
    /**
     * @brief Creates the object
     * 
     * @param width Logical window width
     * @param height Logical window height
     * @param position World coordinates to summon the camera at
     */
    Camera(float width, float height, glm::vec3 position);

    /**
     * @brief Defines the camera viewport
     * 
     * @param FOVdeg Sets the camera's FOV using radians
     * @param nearPlane Models nearer than this will be clipped out
     * @param farPlane Models farther than this will be clipped out
     */
    void setViewport(float FOVdeg, float nearPlane, float farPlane);

    /**
     * @brief Calculates all camera transformation
     * 
     * @param alpha Value for interpolation
     */
    void updateMatrix(double alpha);

    /**
     * @brief Handles event-like user-input like key
     * 
     * @param window Window to get input from
     */
	void updateFixedInput(GLFWwindow* window, double deltaTime); 
    
    /**
     * @brief Updates the camera FOV based on the mouse scroll
     * 
     * @warning This function should be called only after polling events
     */
    
    void updateScroll(double scroll_delta);
    
    /**
     * @brief Sets the camera rotation based on the cursor position
     * 
     * @warning This function should be called only after polling events
     */
    void updateCursor(float dx, float dy);
    
    /**
     * @returns interpolated position vec3 and rotation quaternion
     * 
     * Used for light calculations in the fragment shader
     */
    const Transform* getTransform() const;

    /**
     * @returns interpolated matrix of transformation
     * 
     * Used to set uniform value of the vertex shader
     */
    const glm::mat4* getMatrix() const;

private:
    Transform prev;
    Transform curr;
    Transform intp;

    Frustum frustum;

    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    float mouseDX = 0.0, mouseDY = 0.0;
    float yaw = 0.0f, pitch = 0.0f;
    float speed = 5.0f, sensitivity = 0.33f;
};
#endif