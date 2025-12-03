#include "Model2D.h"

Model2D::Model2D(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, const char* texPath) : 
    vbo(vertices, vertSize, GL_STATIC_DRAW),
    ebo(indices, indSize, GL_STATIC_DRAW),
    texture(texPath, GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE)
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

void Model2D::setPosition(KGL_Vec2f pos) {
    
}

void Model2D::setScale(KGL_Vec2f scale) {
    
}

void Model2D::setRotation(double angle) {
    
}

void Model2D::Draw(ShaderProgram& shader) {
    //int modelLoc = glGetUniformLocation(shader.id, "model");
    vao.Bind();
    texture.Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
void Model2D::Delete() {
    vbo.Delete();
    ebo.Delete();
    vao.Delete();
    texture.Delete();
}