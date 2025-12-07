#ifndef MODEL_CLASS
#define MODEL_CLASS

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

/**
 * @class Model
 * @brief Contains the necessary OpenGL abstractions to render a 3D model with a texture
 * @see Texture
 * @see VBO
 * @see VAO
 * @see EBO
 */
class Model {
public:
    VAO vao;
    VBO vbo;
    EBO ebo;
    Texture texture;
    glm::mat4 modelMatrix;
    GLsizei indexCount;
    ShaderProgram* shader;

    /**
     * @brief Creates a new 3D model
     * @param vertices Array of vertices defining the model geometry
     * @param vertSize Size of the array of vertices
     * @param indices Array of indices defining the model geometry
     * @param indSize Size of the array of indices
     * @param shader Shader program to use in this element
     * @param texPath Path to the model's texture
     * @warning Will throw std::runtime_error if the given vertices, indices, or texture are incorrect 
     * @see Texture
     * @see VBO
     * @see VAO
     * @see EBO
     */
    Model(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, ShaderProgram* shader, const char* texPath);
    /**
     * @brief Translates the object to a given point in the world
     * @param pos 3-dimensional vector for X, Y & Z coordinates
     * @warning The object will not render if outside the camera's view limit
     */
    void setPosition(const glm::vec3& pos);
    /**
     * @brief Sets the model's scale
     * @param scale 3-dimensional vector for X, Y, & Z transformations
     */
    void setScale(const glm::vec3& scale);
    /**
     * @brief Rotates the model the given angle using an anchor
     * @param angle Angle
     * @param axis 3-dimensional vector acting as anchor for X, Y & Z
     */
    void setRotation(float angle, const glm::vec3& axis);
    /**
     * @brief Sends the model's data to the render pipeline
     */
    void Draw();
    /**
     * @brief Frees all allocated resources to this object
     */
    void Delete();
};
#endif