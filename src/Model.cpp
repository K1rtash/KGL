#include "Model.h"

Model::Model(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, const char* texPath) : 
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
    modelMatrix = glm::mat4(1.0f);
}

void Model::setPosition(const glm::vec3& pos) {
    modelMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void Model::setScale(const glm::vec3& scale) {
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Model::setRotation(float angle, const glm::vec3& axis) {
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}

void Model::Draw(ShaderProgram& shader) {
    int modelLoc = glGetUniformLocation(shader.id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    vao.Bind();
    texture.Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
void Model::Delete() {
    vbo.Delete();
    ebo.Delete();
    vao.Delete();
    texture.Delete();
}