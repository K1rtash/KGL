#include "Mesh.h"

#include "glm/gtc/type_ptr.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, glm::mat4 transform) 
	: vertices{vertices}, indices{indices}, textures{textures}, local_trans{transform}
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

struct SamplerNum { /// LLeva la cuenta del id del sampler de textura de esta iteración de el bucle
	unsigned int diffuse, specular;
};

std::string getSamplerName(TextureType type, SamplerNum num) /// Devuelve el nombre del sampler que se tiene que modificar
{
	switch(type) {
		case TextureType::DIFFUSE:
			num.diffuse++;
			return ("diffuse" + std::to_string(num.diffuse));
		case TextureType::SPECULAR:
			num.specular++;
			return ("specular" + std::to_string(num.specular));
	}
	return std::string{};
}

void Mesh::Draw(Shader* shader, Camera* camera, const Transform* transform)
{
	shader->Activate(); // Hay que marcar el programa de shaders y el VAO de este mesh para las llamadas de GL
	vao.Bind();

	SamplerNum samplerN{0};

	for (unsigned int i = 0; i < textures.size(); i++) // Iterar todas las texturas del mesh 
	{
		std::string sampler = getSamplerName(textures[i].type, samplerN); // Nombre del sampler2D

		glUniform1i(shader->GetUniformLoc(sampler), i); // Modifica el sampler2D en el shader
		textures[i].Bind();
	}

	glUniform3fv(shader->GetUniformLoc("camPos"), 1, glm::value_ptr(camera->getTransform().t)); // El vector de posición de la cámara se usa para calculos de luz

	glm::mat4 global = glm::mat4{1.0f};

	global = glm::translate(global, transform->t);
	global *= glm::mat4_cast(transform->r);
	global = glm::scale(global, transform->s);
	global *= local_trans;

	// Ecuación gl_Position
	glUniformMatrix4fv(shader->GetUniformLoc("camMat"), 1, GL_FALSE, glm::value_ptr(camera->getMatrix())); // proj * view
	glUniformMatrix4fv(shader->GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(global)); // model

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}