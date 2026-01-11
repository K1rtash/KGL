#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>
#include <string>

Texture::Texture(unsigned char* bytes, int width, int height, int colorChannels, Type type, unsigned int unit) : type{type}, unit{unit}
{
    try {
        printf("[INFO] Loading texture from data: %dx%d, %d color channels\n", width, height, colorChannels);
        load(bytes, width, height, colorChannels);
    } catch(std::runtime_error& e) {
        std::cout << "[ERROR] while loading texture from data:\n" << e.what() << std::endl;
    }
}

Texture::Texture(std::string path, Type type, unsigned int unit) : path{path}, type{type}, unit{unit}
{
    try {
        printf("[INFO] Loading texture from disc: '%s'\n", path.c_str());
        int w{0}, h{0}, cc{0};
        stbi_set_flip_vertically_on_load(true);
        unsigned char* bytes = stbi_load(path.c_str(), &w, &h, &cc, 0);
        load(bytes, w, h, cc);
        stbi_image_free(bytes);
    } catch(std::runtime_error& e) {
        std::cout << "[ERROR] while loading texture from disc: " << e.what() << std::endl;
    }
}

void Texture::load(unsigned char* bytes, int w, int h, int cc)
{
    if(bytes == nullptr) 
        throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason()) + "\nData: " + std::to_string(w) + "x" + std::to_string(h) + "p " + std::to_string(cc) + "ch");

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format;
    if (cc >= 4) format = GL_RGBA;
    else switch (cc) {
        case 3: format = GL_RGB; break;
        case 2: format = GL_RG; break;
        case 1: format = GL_RED; break;
        default: throw std::runtime_error("invalid channel number: " + std::to_string(cc));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

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
    Unbind();
    glDeleteTextures(1, &id);
    id = 0;
}