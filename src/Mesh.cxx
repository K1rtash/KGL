#include "Mesh.h"

#include "glm/gtc/type_ptr.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, glm::mat4 trans) 
	: vertices{vertices}, indices{indices}, textures{textures}, transform{trans}
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

/// LLeva la cuenta del id del sampler de textura de esta iteración de el bucle
/*struct SamplerNum { 
	unsigned int diffuse, specular;
};*/

/// Devuelve el nombre del sampler que se tiene que modificar
/*std::string getSamplerName(Texture::Type type, SamplerNum num)
{
	switch(type) {
		case KGL_TextureType::DIFFUSE:
			num.diffuse++;
			return ("diffuse" + std::to_string(num.diffuse));
		case KGL_TextureType::SPECULAR:
			num.specular++;
			return ("specular" + std::to_string(num.specular));
	}
	return std::string{};
}*/
#include <iostream>
void Mesh::Draw(Shader* shader, Camera* camera, const Transform* trans)
{
	shader->Activate(); // Hay que marcar el programa de shaders y el VAO de este mesh para las llamadas de GL
	vao.Bind();

	// * TEXTURAS *
	unsigned int diffuse_n = 1, specular_n = 1;
	//SamplerNum samplerN{0};

	for (unsigned int i = 0; i < textures.size(); i++) // Iterar todas las texturas del mesh 
	{
		glActiveTexture(GL_TEXTURE0 + i);
		
		std::string sampler;
		switch(textures[i].type) {
			case Texture::Type::DIFFUSE: 
			sampler = ("diffuse" + std::to_string(diffuse_n++)); 
			diffuse_n++;
				break;
			case Texture::Type::SPECULAR:
			sampler =  ("specular" + std::to_string(specular_n++)); 
			specular_n++;
				break;
		}

		//textures[i].Bind();
		glUniform1i(shader->GetUniformLoc(sampler), i); // Modifica el sampler2D en el shader
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// * ILUMINACIÓN *
	glUniform3fv(shader->GetUniformLoc("camPos"), 1, glm::value_ptr(camera->getTransform().t)); // El vector de posición de la cámara se usa para calculos de luz

	// * TRANSFORMACIÓN *
	glm::mat4 global = glm::mat4{1.0f};
	global = glm::translate(global, trans->t);
	global *= glm::mat4_cast(trans->r);
	global = glm::scale(global, trans->s);
	global *= transform;

	glUniformMatrix4fv(shader->GetUniformLoc("camMat"), 1, GL_FALSE, glm::value_ptr(camera->getMatrix())); // proj * view
	glUniformMatrix4fv(shader->GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(global)); // model

	// * RENDER *
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Delete()
{
	for(unsigned int i = 0; i < textures.size(); i++) textures[i].Delete();
}