#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "Shader.h"

enum class TextureType : unsigned int
{
    DIFFUSE, SPECULAR
};

struct RawTexData {
    std::string file;
    unsigned char* bytes;
    int clrch, width, height;
};

/**
 * @class Texture 
 * @brief GPU-managed 2D texture
 * Loader based on stb_image
 */
class Texture {
public:
    TextureType type;
    GLenum format;
    GLuint id;
    GLuint unit;
    /**
     * @brief Creates a new GPU texture
     * @param data Raw texture data
     * @param slot OpenGL's slot to save a texture (usually 0)
     * @param texType diffuse or specular
     * @note Supported image formats: .PNG .JPG && all supported by stb_image
     */
    Texture(RawTexData* data, GLuint slot, TextureType type);
    /**
     * @brief Calls Delete()
     */
    ~Texture();
    /**
     * @brief Uses this texture on all GL function calls related to this texture's type
     */
    void Bind();
    /**
     * @brief Sets the current texture of this texture type to use to 0
     */
    void Unbind();
    /**
     * @brief Frees all resources associated with this object
     */
    void Delete();
    /**
     * @brief Modify a SAMPLER tipe uniform
     * @param shader Target shader program
     * @param uniform Uniform name
     * @param unit Texture unit
     */
    void texUnit(Shader* shader, const char* uniform, GLuint unit);
    /**
     * @brief Interprets a disc file into raw byte data
     * @param data Object to save data in
     */
    static int resolveData(RawTexData* data); 
};
#endif