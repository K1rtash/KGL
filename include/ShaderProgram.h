#ifndef SHADER_PROGRAM_CLASS
#define SHADER_PROGRAM_CLASS

#include <glad/glad.h>
#include <string>
#include <unordered_map>

class ShaderProgram 
{
    const char* vertPath;
    const char* fragPath;
    std::unordered_map<std::string, int>uniformLocations;
    void linkShaders(GLuint vertex_shader, GLuint fragment_shader);
public:
    int UMAT4_LOC_PROJECTION, UMAT4_LOC_VIEW, UMAT4_LOC_MODEL, USAM2D_TEX0; 
    GLuint id;
    /**
     * @brief Creates a new shader program consisting of a Vertex and Fragment shader
     * @param vertex_shader_path Source file in GLSL
     * @param fragment_shader_path Source file in GLSL
     * @warning Errors in the shaders will throw std::runtime_error
     */
    ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
    /**
     * @brief Get the memory address of a uniform in the shaders
     * @param name Uniform id
     * @returns Memory address as integer
     */
    int GetUniformLoc(const std::string& name) { return uniformLocations[name]; }
    /**
     * @brief Locates and saves the memory address of a non-static uniform
     * @param name Uniform id
     */
    void AddUniform(const char* name);
    /**
     * @brief Compiles all shaders from source and sets all uniform locations
     * @warning Compile-time GLSL errors will throw std::runtime_error
     */
    void Reload();
    /**
     * @brief Frees all resources allocated in this object
     */
    void Delete();
    /**
     * @brief Binds this shader for all OpenGL function calls 
     */
    void Activate();
};
#endif