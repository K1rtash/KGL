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
     * @brief Links a vertex attribute from a VBO to a shader input variable.
     *
     * This function configures how the vertex data stored in a VBO is interpreted
     * and associates it with a specific attribute location in the shader program
     * using glVertexAttribPointer. It is essential for defining the vertex layout
     * (e.g., positions, colors, normals, texture coordinates) that will be passed
     * into the graphics pipeline.
     *
     * @param vbo           Pointer to the Vertex Buffer Object containing vertex data.
     * @param layout        Attribute location index in the shader (obtained via glGetAttribLocation).
     * @param numComponents Number of components per attribute (e.g., 3 for vec3, 2 for vec2).
     * @param type          Data type of each component (e.g., GL_FLOAT, GL_INT).
     * @param stride        Byte offset between consecutive attributes in the VBO.
     * @param offset        Offset in the buffer where the attribute data begins.
     *
     * @note The VBO must be bound before calling this function.
     * @see glVertexAttribPointer, glEnableVertexAttribArray
     */
    void LinkAttrib(VBO* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
};
#endif