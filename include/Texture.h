#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "ShaderProgram.h"

class Texture {
    int width, height;
    GLenum format;
public:
    GLuint id;
    GLenum type;
    Texture(const char* path, GLenum texType, GLenum slot, GLenum pixelType);
    void Bind();
    void Unbind();
    void Delete();
    void texUnit(ShaderProgram* shader, const char* uniform, GLuint unit);
};
#endif