#include "Texture.h"
#include <stb/stb_image.h>
#include <iostream>
#include <string>

Texture::Texture(const char* path, GLenum texType, GLuint slot, GLenum pixelType) : type{texType}, unit{slot} 
{
    int numColCh;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* bytes = stbi_load(path, &width, &height, &numColCh, 0);

    std::cout << "Loading image '" << path << "' (" << width << "x" << height << ") colorch: " << numColCh << std::endl;
    
    if(bytes == nullptr) throw std::runtime_error("ERROR::TEXTURE::LOAD_FAILED (" + std::string(path) + ")");
    switch(numColCh) 
    {
        case 0: throw std::runtime_error("ERROR::TEXTURE::UNSUPPORTED_NUMBER_OF_CHANNELS (" + std::string(path) + ")");
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        default: format = GL_RGBA;
    }

    glGenTextures(1, &id); // Generates an OpenGL texture object
    glActiveTexture(GL_TEXTURE0 + slot); // Assigns the texture to a Texture Unit
    Bind();

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(texType, 0, format, width, height, 0, format, pixelType, bytes); // Assigns the image to the OpenGL Texture object
    glGenerateMipmap(texType); // Generates MipMaps

    stbi_image_free(bytes);
    Unbind();
}

void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(type, id);
}

void Texture::Unbind() {
    glBindTexture(type, 0);
}
void Texture::Delete() {
    glDeleteTextures(1, &id);
    id = 0;
}

void Texture::texUnit(ShaderProgram* shader, const char* uniform, GLuint unit) 
{
    shader->Activate(); // Shader needs to be activated before changing the value of a uniform
    glUniform1i(glGetUniformLocation(shader->id, uniform), unit); // Sets the value of the uniform
}

Texture::~Texture() {
    //Delete();
}