#include "EBO.h"

EBO::EBO(GLuint* indices, size_t size, GLenum usage) : usage{usage} {
    glGenBuffers(1, &id);
    Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
}

void EBO::Bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
};

void EBO::Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void EBO::Delete() {
    glDeleteBuffers(1, &id);
};