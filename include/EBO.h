#ifndef EBO_CLASS
#define EBO_CLASS

#include <glad/glad.h>

class EBO {
private:
    GLuint usage;
public:
    GLuint id;
    EBO(GLuint* indices, size_t size, GLenum usage);
    void Bind();
    void Unbind();
    void Delete();
};
#endif