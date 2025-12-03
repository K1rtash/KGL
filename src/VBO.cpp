#include "VBO.h"

VBO::VBO(GLfloat* vertices, size_t size, GLenum usage) : usage{usage} {
    glGenBuffers(1, &id);
    Bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
}

void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
};

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
};

void VBO::Delete() {
    glDeleteBuffers(1, &id);
};