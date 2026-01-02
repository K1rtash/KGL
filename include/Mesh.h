#ifndef MESH_CLASS
#define MESH_CLASS

#include <vector>
#include <glm/glm.hpp>

#include "VAO.h"
#include "EBO.h"
#include "VBO.h"
#include "Camera.h"
#include "Texture.h"

using std::vector, std::string;

class Mesh 
{
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    glm::mat4 transform; /// Transformacion de este mesh en su modelo 

    VAO vao;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, glm::mat4 trans = glm::mat4{1.0f});
    void Draw(Shader*, Camera*, const Transform*);
    void Delete();
};
#endif