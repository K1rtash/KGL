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
    const float aspect, width, height;
    bool firstClick = true;
    ShaderProgram* shader;
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera(float width, float height, glm::vec3 position, ShaderProgram* shader);
	void Matrix(float FOVdeg, float nearPlane, float farPlane);
	void Inputs(GLFWwindow* window, double deltaTime); 
};
#endif