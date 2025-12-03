#ifndef VBO_CLASS
#define VBO_CLASS

#include <glad/glad.h>

class VBO {
private:
    GLuint usage;
public:
    GLuint id;
    VBO(GLfloat* vertices, size_t size, GLenum usage);
    void Bind();
    void Unbind();
    void Delete();
};
#endif