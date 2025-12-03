#ifndef MODEL_CLASS
#define MODEL_CLASS

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

class Model {
public:
    VAO vao;
    VBO vbo;
    EBO ebo;
    Texture texture;
    glm::mat4 modelMatrix;
    GLsizei indexCount;

    Model(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indSize, const char* texPath);
    void setPosition(const glm::vec3& pos);
    void setScale(const glm::vec3& scale);
    void setRotation(float angle, const glm::vec3& axis);
    void Draw(ShaderProgram& shader);
    void Delete();
};
#endif