#include "Shader.h"
#include "Kirtash/stl.h"

#include <stdexcept>
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>

using std::string, std::ifstream, std::stringstream, std::cerr, std::endl;

std::string read_file_contents(std::string filepath) 
{
    ifstream file(filepath);

    if (!file.is_open()) throw std::runtime_error("unable to open source file '" + filepath + "'");
    if (file.fail() || file.bad()) throw std::runtime_error("cant read source file '" + filepath + "'");

    stringstream buffer;
    buffer << file.rdbuf();

    if (buffer.str().empty()) throw std::runtime_error("source file is empty '" + filepath + "'");

    return buffer.str();
}

GLuint compileShaderSource(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        switch(type) {
            case GL_VERTEX_SHADER:
                throw std::runtime_error(std::format("VERTEX_SHADER::COMPILATION_FAILED\n{}", infoLog));
                break;
            case GL_FRAGMENT_SHADER:
                throw std::runtime_error(std::format("FRAGMENT_SHADER::COMPILATION_FAILED\n{}", infoLog));
                break;
            default:
                throw std::runtime_error(std::format("UNKNOWN_SHADER::COMPILATION_FAILED\n{}", infoLog));
        }
    } 
    return shader;
}

void Shader::create() 
{
    int uniformCount = 0;

    linkShaders(
        compileShaderSource(read_file_contents(vertPath).c_str(), GL_VERTEX_SHADER),
        compileShaderSource(read_file_contents(fragPath).c_str(), GL_FRAGMENT_SHADER)
    );

    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (int i = 0; i < uniformCount; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;

        glGetActiveUniform(id, i, sizeof(name), &length, &size, &type, name);

        GLint location = glGetUniformLocation(id, name);

        uniformLocations[std::string(name)] = location;
    }
}

int Shader::GetUniformLoc(const std::string& name) {
    auto i = uniformLocations.find(name);
    if(i == uniformLocations.end()) {
        std::cout << "[WARNING] tried to access unmapped uniform location: " << name << " (in shader id: " << id << ")" << std::endl;
        uniformLocations[name] = -1;
        return -1;
    }
    return i->second;
}

void Shader::linkShaders(GLuint vertex_shader, GLuint fragment_shader) 
{
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        throw std::runtime_error(std::format("ERROR::SHADER_PROGRAM::LINKING_FAILED\n{}", infoLog));
    }

    glUseProgram(id);
}

Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path) : id{glCreateProgram()} 
{
    vertPath = kirtash::normalizeString(vertex_shader_path);
    fragPath = kirtash::normalizeString(fragment_shader_path);

    try {
        std::cout << "[INFO] creating shader program with id: " << id << " vertex shader: '" << vertPath << "' fragment shader: '" << fragPath << "'" << std::endl;
        create();
    } catch (std::runtime_error& e) {
        std::cout << "[ERROR] while creating shader program: " << e.what() << std::endl;
    }

    for (auto& [name, loc] : uniformLocations) {
    std::cout << "[INFO] mapped uniform '" << name << "' at loc " << loc << " (in shader id: " << id << ")" << std::endl;
}

}

void Shader::Reload() {
    Delete();
    id = glCreateProgram();
    std::cout << "[INFO] Reloading shaders with id: " << id << " from source. Vertex shader: '" << vertPath << "' Fragment shader: '" << fragPath << "'" << std::endl;
    try {
        create();
    } catch (std::runtime_error& e) {
        std::cout << "[ERROR] while reloading shader program: " << e.what() << std::endl;
    }
}

void Shader::Delete() {
    glUseProgram(0);
    glDeleteProgram(id);
    id = 0;
}

void Shader::Activate() {
    glUseProgram(id);
}