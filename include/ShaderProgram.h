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
    ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
    int GetUniformLoc(const std::string& name) { return uniformLocations[name]; }
    void AddUniform(const char* name);
    void Reload();
    void Delete();
    void Activate();
};
#endif