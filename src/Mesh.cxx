#include "Mesh.h"
#include "iostream"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) : vertices{vertices}, indices{indices}, textures{textures}
{
    vao.Bind();
    VBO vbo{&vertices};
    EBO ebo{&indices};
    vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); 				  // Position
    vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Normals
    vao.LinkAttrib(&vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // TexCoords
    vao.Unbind();
    ebo.Unbind();
    vbo.Unbind();
}

void Mesh::Draw(Shader* shader, Camera* camera, const Transform* transform)
{
	shader->Activate();// Bind shader to be able to access uniforms
	vao.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string sampler;
		TextureType type = textures[i].type;
		if (type == TextureType::DIFFUSE)
		{
			num = std::to_string(numDiffuse++);
			sampler = ("diffuse" + num);
		}
		else if (type == TextureType::SPECULAR)
		{
			num = std::to_string(numSpecular++);
			sampler = ("specular" + num);
		}
		glUniform1i(shader->GetUniformLoc(sampler), i); //modify sampler2D
		textures[i].Bind();
	}

	glUniform3fv(shader->GetUniformLoc("camPos"), 1, glm::value_ptr(camera->getTransform()->pos));
	glUniformMatrix4fv(shader->GetUniformLoc("camMat"), 1, GL_FALSE, glm::value_ptr(camera->cameraMatrix));

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, transform->t);
	model *= glm::mat4_cast(transform->r);
	model = glm::scale(model, transform->s);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(shader->GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(model));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}