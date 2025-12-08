#ifndef TEXTURE_2D_CLASS
#define TEXTURE_2D_CLASS

#include <glad/glad.h>
#include "ShaderProgram.h"

/**
 * @class Texture 
 * @brief GPU-managed 2D texture
 * Loader based on stb_image
 */
class Texture {
    int width, height;
    GLenum format;
public:
    GLuint id;
    GLenum type;
    /**
     * @brief Creates a new GPU texture
     * @param path Path to the asset
     * @param texType OpenGL's 2D or 3D texture type
     * @param slot OpenGL's slot to save a texture (usually 0)
     * @param pixelType Data type of the texture pixels (as in unsigned byte)
     * @warning Will throw a std::runtime_error if the path cant be resolved or if the image format is not supported
     * @note Supported image formats: .PNG .JPG && all supported by stb_image
     */
    Texture(const char* path, GLenum texType, GLenum slot, GLenum pixelType);
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
    void texUnit(ShaderProgram* shader, const char* uniform, GLuint unit);
};
#endif