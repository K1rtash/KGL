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
    glm::mat4 local_trans;

    VAO vao;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, glm::mat4 local_trans = glm::mat4{1.0f});
    void Draw
	(
		Shader* shader, 
		Camera* camera,
		const Transform* transform
	);
};
#endif