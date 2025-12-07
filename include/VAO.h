#ifndef VAO_CLASS
#define VAO_CLASS

#include <glad/glad.h>
#include "VBO.h"

/**
 * @class VAO
 * @brief Abstraction of OpenGL's Vertex Array Object
 * Relates an Vertex Buffer Object and an optional Element Buffer Object, allowing to link attributes
 * @see VBO
 * @see EBO
 */
class VAO {
public:
    GLuint id;
    /**
     * @brief Creates a new Vertex Array Object
     */
    VAO();
    /**
     * @brief Makes OpenGL use this object as the current VAO for all function calls
     */
    void Bind();
    /**
     * @brief Unsets the VAO
     */
    void Unbind();
    /**
     * @brief Free all allocated resources to this VAO
     */
    void Delete();
    /**
     * @brief Defines how to use a layout of elemts as an attribute that will be passed to the shader
     * @param vbo Vertex Buffer Object that contains the vertices data
     * @param layout Location of the shader IN variable
     * @param numComponents Number of elements per attribute
     * @param type Data type of the elements
     * @param stride Lenght in bytes of the attribute
     * @param offset First element to start the attribute, 0 being the start of the array
     */
    void LinkAttrib(VBO* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
};
#endif