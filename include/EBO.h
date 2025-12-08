#ifndef EBO_CLASS
#define EBO_CLASS

#include <glad/glad.h>
#include <vector>

/**
 * @class EBO
 * @brief Abstracted OpenGL Element Buffer Object
 *
 * Saves indices of vertices of an element
 * @see VBO
 */
class EBO {
private:
    GLuint usage;
public:
    GLuint id;
    /**
     * @brief Creates a new Element Buffer Object
     * @param indices Array of indices
     * @param size Size in bytes of the array
     * @param usage OpenGL usage mode of this EBO
     * @note usage may be GL_STATIC_DRAW if the indices will not change or GL_DYNAMIC_DRAW if the indices will change often
     */
    EBO(std::vector<GLuint>& indices);
    /**
     * @brief Makes OpenGL use this object as the current EBO for all function calls
     */
    void Bind();
    /**
     * @brief Unsets the EBO
     */
    void Unbind();
    /**
     * @brief Free all allocated resources to this EBO
     */
    void Delete();
};
#endif