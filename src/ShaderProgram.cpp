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

void ShaderProgram::getUniformsAndLinkShaders() {
    linkShaders(
        compileShaderSource(loadShaderSource(vertPath).c_str(), GL_VERTEX_SHADER),
        compileShaderSource(loadShaderSource(fragPath).c_str(), GL_FRAGMENT_SHADER));
    for (auto& [name, location] : uniformLocations) {
        location = glGetUniformLocation(this->id, name.c_str());
        std::cout << "uniform locationsfor shaderid: " << id << " -> " << "name: " << name << " loc: " << location << std::endl;
    }
}

void ShaderProgram::AddUniform(const char* name) {
    uniformLocations[name] = glGetUniformLocation(id, name);
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
        getUniformsAndLinkShaders();
    } catch (std::runtime_error& e) {
        std::cout << "Error while creating shader program: " << e.what() << std::endl;
    }
}

void ShaderProgram::Reload() {
    Delete();
    id = glCreateProgram();
    try {
        getUniformsAndLinkShaders();
    } catch (std::runtime_error& e) {
        std::cout << "Error while reloading shader program: " << e.what() << std::endl;
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