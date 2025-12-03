#ifndef MODEL2D_CLASS
#define MODEL2D_CLASS

#include <glad/glad.h>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Vector.h"

class Model2D 
{
    VAO vao;
    VBO vbo;
    EBO ebo;
    Texture texture;
    GLsizei indexCount;
    KGL_Vec2f pos;

public:
    Model2D(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, const char* texPath);
    void setPosition(KGL_Vec2f pos);
    void setScale(KGL_Vec2f scale);
    void setRotation(double angle);
    void Draw(ShaderProgram& shader);
    void Delete();
};

#endif