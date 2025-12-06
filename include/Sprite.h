#ifndef SPRITE_CLASS
#define SPRITE_CLASS

#include <glad/glad.h>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include <glm/glm.hpp>

class Sprite 
{
    VAO vao;
    VBO vbo;
    EBO ebo;
    Texture texture;
    GLsizei indexCount;
    ShaderProgram* shader;
    struct Transforms {
        float 
        x = 0.0f, y = 0.0f, 
        scaleX = 1.0f, scaleY = 1.0f, 
        rotation = 0.0f;
    } transforms;
    glm::mat4 model = glm::mat4(1.0f);

    float width_px, height_px;
    
public:
    Sprite(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize,  ShaderProgram* shader, const char* texPath, int width, int height);
    void setPosition(float x, float y);
    void setScale(float s);
    void setRotation(float angle);
    void Draw(int screenW, int screenH);
    void Delete();
};

#endif