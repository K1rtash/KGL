#ifndef VAO_CLASS
#define VAO_CLASS

#include <glad/glad.h>
#include "VBO.h"

class VAO {
public:
    GLuint id;
    VAO();
    void Bind();
    void Unbind();
    void Delete();
    void LinkAttrib(VBO* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
};
#endif