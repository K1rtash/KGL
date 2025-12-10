#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "Shader.h"

/**
 * @class Texture 
 * @brief GPU-managed 2D texture
 * Loader based on stb_image
 */
class Texture {
public:
    int width, height;
    const char* type;
    GLenum format;
    GLuint id;
    GLuint unit;
    /**
     * @brief Creates a new GPU texture
     * @param path Path to the asset
     * @param texType to-do
     * @param slot OpenGL's slot to save a texture (usually 0)
     * @param pixelType Data type of the texture pixels (as in unsigned byte)
     * @warning Will throw a std::runtime_error if the path cant be resolved or if the image format is not supported
     * @note Supported image formats: .PNG .JPG && all supported by stb_image
     */
    Texture(const char* path, const char* texType, GLuint slot, GLenum format, GLenum pixelType);
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
     * @note to-do
     */
    void texUnit(Shader* shader, const char* uniform, GLuint unit);
};
#endif