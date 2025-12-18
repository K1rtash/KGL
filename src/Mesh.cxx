#include "Mesh.h"

Mesh::Mesh(vector<Vertex>* vertices, vector<GLuint>* indices, vector<Texture>* textures) : vertices{*vertices}, indices{*indices}, textures{*textures}
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

void Mesh::Draw(Shader* shader, Camera* camera, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	shader->Activate();// Bind shader to be able to access uniforms
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
        textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	glUniform3f(shader->GetUniformLoc("camPos"), camera->intp.pos.x, camera->intp.pos.y, camera->intp.pos.z);
	glUniformMatrix4fv(shader->GetUniformLoc("camMatrix"), 1, GL_FALSE, glm::value_ptr(camera->cameraMatrix));

	// Initialize matrices
	glm::mat4 t = glm::mat4(1.0f), r = glm::mat4(1.0f), s = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	t = glm::translate(t, translation);
	r = glm::mat4_cast(rotation);
	s = glm::scale(s, scale);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(shader->GetUniformLoc("translation"), 1, GL_FALSE, glm::value_ptr(t));
	glUniformMatrix4fv(shader->GetUniformLoc("rotation"), 1, GL_FALSE, glm::value_ptr(r));
	glUniformMatrix4fv(shader->GetUniformLoc("scale"), 1, GL_FALSE, glm::value_ptr(s));
	glUniformMatrix4fv(shader->GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(matrix));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}