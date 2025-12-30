#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "Shader.h"

enum class TextureType : unsigned int
{
    DIFFUSE, SPECULAR
};

struct RawTexData 
{
    unsigned char* bytes;
    int clrch, width, height;
};

/**
 * @brief Interprets a disc image into raw texture data
 * @param path Image location in disc
 * @returns Texture raw data @ref RawTexData
 */
RawTexData getDiscFileData(const char* file);

/**
 * @brief Interprets embedded byte data into raw texture data
 * 
 * @param bytes Image bytes
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param colorCh Image color channels
 * @returns Texture raw data @ref RawTexData
 */
RawTexData getEmbeddedData(unsigned char* bytes, int width, int height, int colorCh);

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
    std::string path;
    /**
     * @brief Creates a new GPU texture
     * 
     * This function will free memory allocated to the raw texture data pixel bytes
     * 
     * @param data @ref ByteData
     * @param slot OpenGL's slot to save a texture (usually 0)
     * @param texType @ref TextureType
     */
    Texture(RawTexData data, GLuint slot, TextureType type);

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
};
#endif