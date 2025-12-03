#ifndef SHADER_PROGRAM_CLASS
#define SHADER_PROGRAM_CLASS

#include <glad/glad.h>
#include <string>

class ShaderProgram 
{
    const char* vertPath;
    const char* fragPath;
    void linkShaders(GLuint vertex_shader, GLuint fragment_shader);
public:
    GLuint id;
    ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
    int GetUniformLoc(const char* name) { return glGetUniformLocation(id, name); }
    void Reload();
    void Delete();
    void Activate();
};
#endif