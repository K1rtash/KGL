#ifndef VBO_CLASS
#define VBO_CLASS

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Transform 
{
	glm::vec3 t;
	glm::quat r;
	glm::vec3 s;
};

/**
 * @class VBO
 * @brief Abstraction of OpenGL's Vertex Buffer Object
 * Saves vertices in GPU memory
 * @see EBO
 * @see VAO
 */
class VBO {
private:
    GLuint usage;
public:
    GLuint id;
    /**
     * @brief Creates a new Vertex Buffer Object
     * @param vertices Array of vertices
     * @param usage OpenGL usage mode of this VBO
     * @note usage may be GL_STATIC_DRAW if the vertices will not change or GL_DYNAMIC_DRAW if the vertices will change often
     */
    VBO(std::vector<Vertex>* vertices);
    /**
     * @brief Makes OpenGL use this object as the current VBO for all function calls
     */
    void Bind();
    /**
     * @brief Unsets the VBO
     * @warning Calling OpenGL VBO functions whithout binding another object will cause errors and crashes
     */
    void Unbind();
    /**
     * @brief Free all allocated resources to this VBO
     */
    void Delete();
};
#endif