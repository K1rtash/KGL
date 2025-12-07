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
    /**
     * @brief Creates a new 2D sprite
     * @param vertices Array of vertices defining the model geometry
     * @param vertSize Size of the array of vertices
     * @param indices Array of indices defining the model geometry
     * @param indSize Size of the array of indices
     * @param shader Shader program to use in this element
     * @param texPath Path to the model's texture
     * @param width Width in pixels
     * @param height Height in pixels
     * @warning Will throw std::runtime_error if the given vertices, indices, or texture are incorrect
     * @see Texture
     * @see VBO
     * @see VAO
     * @see EBO
     */
    Sprite(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, ShaderProgram* shader, const char* texPath, int width, int height);
    void setPosition(float x, float y);
    void setScale(float s);
    void setRotation(float angle);
    void Draw(int screenW, int screenH);
    void Delete();
};

#endif