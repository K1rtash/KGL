#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "Shader.h"


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
//LEGACY

/**
 * @class Texture 
 * @brief GPU-managed 2D texture
 * Loader based on stb_image
 */
class Texture {
public:
    GLuint id, unit;
    std::string path;

    /**
     * @brief Uniform sampler2D type for PBR
     */
    enum Type : unsigned int{
        DIFFUSE, SPECULAR, NORMAL, HEIGHT, METALLIC, ROUGHNESS, METALLIC_ROUGHNESS, AO, EMISSIVE, BASE_COLOR
    } type;

    /**
     * @param bytes Embedded data
     * @param width Image width in pixels
     * @param height Image height in pixels
     * @param colorChannels Image color channels
     * @param type Material type @ref Type
     * @warning You must free byte data after calling this function!
     */
    Texture(unsigned char* bytes, int width, int height, int colorChannels, Type type, unsigned int unit);

    /**
     * @param path Disc file path
     * @param type Material type @ref Type
     */
    Texture(std::string path, Type type, unsigned int unit);

    Texture(RawTexData data, GLuint slot, Type type); //constructor legacy

    ~Texture();

    /**
     * @brief Binds the texture and activates the texture unit
     * 
     * Use before
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

private:
    /**
     * @brief Resolves texture data
     * @param bytes RGBA byte data 
     * @param w Width 
     * @param h Height 
     * @param cc Color channels 
     */
    void load(unsigned char* bytes, int w, int h, int cc);
};
#endif