#include "Mesh.h"

Mesh::Mesh(vector<Vertex>& vertices, vector<GLuint>& indices, vector<Texture>& textures) : vertices{vertices}, indices{indices}, textures{textures}
{
    vao.Bind();
    VBO vbo{vertices};
    EBO ebo{indices};
    vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    vao.LinkAttrib(&vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
    vao.LinkAttrib(&vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

    vao.Unbind();
    ebo.Unbind();
    vbo.Unbind();
}

/*void Mesh::Draw(Shader* shader, Camera* camera)
{
    shader->Activate();
    vao.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for(unsigned int i = 0; i < textures.size(); i++) 
    {
        std::string num;
        std::string type = textures[i].type;
        if(type == "diffuse") {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "specular") {
            num = std::to_string(numSpecular++);
        }
        textures[i].texUnit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }

    glUniform3f(shader->GetUniformLoc("camPos"), camera->Position.x, camera->Position.y, camera->Position.z); //camara del jugador
    camera->Matrix(shader, "cameraMatrix");

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}*/

void Mesh::Draw(Shader& shader, Camera& camera)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	vao.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		//textures[i].texUnit(&shader, (type + num).c_str(), i);
        textures[i].texUnit(shader, (type + num).c_str(), i);
        //glUniform1i(glGetUniformLocation(shader.id, (type + num).c_str()), i);
		textures[i].Bind();
	}
	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.id, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}