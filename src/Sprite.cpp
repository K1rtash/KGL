#include "Sprite.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Sprite::Sprite(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize,  ShaderProgram* shader, Texture* texture, int width, int height, float screenWidth, float screenHeight) : 
    vbo(vertices, vertSize, GL_STATIC_DRAW),
    ebo(indices, indSize, GL_STATIC_DRAW),
    texture(texture),
    shader(shader), 
    width_px(width), height_px(height),
    LOGICAL_SCREEN_WIDTH(screenWidth), LOGICAL_SCREEN_HEIGHT(screenHeight)
{
    vao.Bind();
    vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkAttrib(&vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    indexCount = indSize / sizeof(GLuint);   
}

void Sprite::setPosition(float x, float y) {
    transforms.x = x;
    transforms.y = y;
}

void Sprite::setScale(float s) {
    transforms.scaleX = s;
    transforms.scaleY = s;
}

void Sprite::setRotation(float angle) {
    transforms.rotation = angle;
}

void Sprite::Draw() {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(transforms.x, transforms.y, 0.0f));
    model = glm::rotate(model, glm::radians(transforms.rotation), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::scale(model, glm::vec3(transforms.scaleX * (float)width_px, transforms.scaleY * (float)height_px, 1.0f));

    glm::mat4 view = glm::mat4(1.0f); // cámara vacía
    glm::mat4 proj = glm::ortho(
        -LOGICAL_SCREEN_WIDTH / 2.0f, LOGICAL_SCREEN_WIDTH / 2.0f,
        -LOGICAL_SCREEN_HEIGHT / 2.0f, LOGICAL_SCREEN_HEIGHT / 2.0f,
        -1.0f, 1.0f
    );

    glUniformMatrix4fv(shader->GetUniformLoc("cameraMatrix"), 1, GL_FALSE, glm::value_ptr(proj * view));
    glUniformMatrix4fv(shader->GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(model));

    vao.Bind();

    if (texture != nullptr) {
        texture->texUnit(shader, "tex0", 0);
        texture->Bind();
        glUniform1i(shader->GetUniformLoc("useTexture"), true);
    } else {
        glUniform1i(shader->GetUniformLoc("useTexture"), false);
    }

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Sprite::Delete() {
    vbo.Delete();
    ebo.Delete();
    vao.Delete();
    if(texture != nullptr) texture->Delete();
}