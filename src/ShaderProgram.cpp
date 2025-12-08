#include "ShaderProgram.h"
#include <stdexcept>
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>

using std::string, std::ifstream, std::stringstream, std::cerr, std::endl;

string loadShaderSource(const char* filepath) 
{
    ifstream file(filepath);

    if (!file.is_open()) throw std::runtime_error("Unable to open source file");
    if (file.fail() || file.bad()) throw std::runtime_error("Cant reading source file");


    stringstream buffer;
    buffer << file.rdbuf();

    if (buffer.str().empty()) throw std::runtime_error("Source file is empty");

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

void ShaderProgram::create() 
{
    int uniformCount = 0;

    linkShaders(
        compileShaderSource(loadShaderSource(vertPath).c_str(), GL_VERTEX_SHADER),
        compileShaderSource(loadShaderSource(fragPath).c_str(), GL_FRAGMENT_SHADER)
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

void ShaderProgram::AddUniform(const char* name) {
    uniformLocations[name] = glGetUniformLocation(id, name);
}

int ShaderProgram::GetUniformLoc(const std::string& name) {
    auto i = uniformLocations.find(name);
    if(i == uniformLocations.end()) {
        std::cout << "[WARNING] unmapped uniform location: " << name << " (in shader id: " << id << ")" << std::endl;
        return -1;
    }
    return i->second;
}

void ShaderProgram::linkShaders(GLuint vertex_shader, GLuint fragment_shader) 
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

ShaderProgram::ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path) : 
    id{glCreateProgram()}, 
    vertPath{vertex_shader_path}, 
    fragPath{fragment_shader_path} 
{
    try {
        create();
    } catch (std::runtime_error& e) {
        std::cout << "[ERROR] while creating shader program: " << e.what() << std::endl;
    }
}

void ShaderProgram::Reload() {
    Delete();
    id = glCreateProgram();
    try {
        create();
    } catch (std::runtime_error& e) {
        std::cout << "[ERROR] while reloading shader program: " << e.what() << std::endl;
    }
}

void ShaderProgram::Delete() {
    glUseProgram(0);
    glDeleteProgram(id);
    id = 0;
}

void ShaderProgram::Activate() {
    glUseProgram(id);
}