#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>
#include <string>

RawTexData getDiscFileData(const char* file)
{
    RawTexData data{
        .bytes = nullptr, 
        .clrch = 0, 
        .width = 0, 
        .height = 0,
    };

    stbi_set_flip_vertically_on_load(true);
    data.bytes = stbi_load(file, &(data.width), &(data.height), &(data.clrch), 0);
    
    printf("[INFO] Loading texture from disc '%s' (%ix%i %i-ch)\n", file, data.width, data.height, data.clrch);

    return data;
}

RawTexData getEmbeddedData(unsigned char* bytes, int width, int height, int colorCh)
{
    RawTexData data{
        .bytes = bytes, 
        .clrch = colorCh, 
        .width = width, 
        .height = height, 
    };
    
    return data;
}

Texture::Texture(RawTexData data, GLuint slot, Type type) : type{type}, unit{slot} 
{
    if (data.bytes == nullptr) throw std::runtime_error("null ptr to texture byte data (tex slot: " + std::to_string(slot) + ")");

    glGenTextures(1, &id);
    Bind();

    // Configures the type of algorithm that is used to make the image smaller or bigger (MIPMAP)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint format;
    if (data.clrch >= 4) 
        format = GL_RGBA;
    else 
    {
        switch ( data.clrch ) {
            case 3: format = GL_RGB; break;
            case 2: format = GL_RG; break;
            case 1: format = GL_RED; break;
            default: throw std::runtime_error("invalid texture channel number: " + std::to_string(data.clrch));
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.bytes); // Assigns the image to the OpenGL Texture object
    glGenerateMipmap(GL_TEXTURE_2D); // Generates MipMaps

    stbi_image_free(data.bytes);
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