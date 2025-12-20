#include "Texture.h"
#include <stb/stb_image.h>
#include <iostream>
#include <string>

int Texture::resolveData(RawTexData* data) 
{
    stbi_set_flip_vertically_on_load(true);
    data->bytes = stbi_load(data->file.c_str(), &(data->width), &(data->height), &(data->clrch), 0);

    std::cout << "[INFO] Loading texture from disc '" << data->file << "' (" << data->width << "x" << data->height << ") colorch: " << data->clrch << std::endl;
    if(data->clrch > 4) data->clrch = 4;

    return (data->bytes != nullptr);
}

Texture::Texture(RawTexData* data, GLuint slot, TextureType type) : type{type}, unit{slot} 
{
    glGenTextures(1, &id);
    Bind();

    // Configures the type of algorithm that is used to make the image smaller or bigger (MIPMAP)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data->clrch >= 4) 
        format = GL_RGBA;
    else 
    {
        switch ( data->clrch ) {
            case 3: format = GL_RGB; break;
            case 2: format = GL_RG; break;
            case 1: format = GL_RED; break;
            default: throw std::runtime_error("invalid texture channel number: " + std::to_string(data->clrch));
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->width, data->height, 0, format, GL_UNSIGNED_BYTE, data->bytes); // Assigns the image to the OpenGL Texture object
    glGenerateMipmap(GL_TEXTURE_2D); // Generates MipMaps

    stbi_image_free(data->bytes);
    Unbind();
}

void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::Delete() {
    glDeleteTextures(1, &id);
    id = 0;
}

Texture::~Texture() {
    //Delete();
}

void Texture::texUnit(Shader* shader, const char* uniform, GLuint unit)
{
	glUniform1i(shader->GetUniformLoc(uniform), unit); 	// Shader needs to be activated before changing the value of a uniform
}