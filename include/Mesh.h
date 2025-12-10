#ifndef MESH_CLASS
#define MESH_CLASS

#include <string>
#include <vector>
#include "VAO.h"
#include "EBO.h"
#include "VBO.h"
#include "Camera.h"
#include "Texture.h"

using std::vector, std::string;

/**
 * @class Model
 * @brief Contains the necessary OpenGL abstractions to render a 3D model with a texture
 * @see Texture
 * @see VBO
 * @see VAO
 * @see EBO
 */
class Mesh 
{
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    VAO vao;

    Mesh(vector<Vertex>* vertices, vector<GLuint>* indices, vector<Texture>* textures);
    void Draw(Shader* shader, Camera* camera);
};
#endif