#ifndef SHADER_PROGRAM_CLASS
#define SHADER_PROGRAM_CLASS

#include <glad/glad.h>
#include <string>
#include <unordered_map>

/**
 * @brief Parses a file into a string
 * @param filepath Path to the file
 * @returns String containing raw text
 */
std::string read_file_contents(const char* filepath); 

class Shader 
{
    const char* vertPath;
    const char* fragPath;
    std::unordered_map<std::string, int>uniformLocations;
    void linkShaders(GLuint vertex_shader, GLuint fragment_shader);
    void create();
public:
    GLuint id;
    /**
     * @brief Creates a new shader program consisting of a Vertex and Fragment shader
     * @param vertex_shader_path Source file in GLSL
     * @param fragment_shader_path Source file in GLSL
     * @warning Errors in the shaders will throw std::runtime_error
     */
    Shader(const char* vertex_shader_path, const char* fragment_shader_path);
    /**
     * @brief Get the memory address of a uniform in the current shader program
     * @param name Uniform name
     * @returns Memory address as integer
     * @note If the uniform can't be located this function will return -1 and throw a warning in the std ostream
     */
    int GetUniformLoc(const std::string& name);
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