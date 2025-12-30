#include "Model.h"
#include "iostream"

void Model::Draw(Shader* shader, Camera* camera, Transform transform)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader, camera, &transform);
}  

void loadTexture()
{
    try {
        Texture texture{getDiscFileData("../textures/bricks.png"), 0, TextureType::DIFFUSE};
    } catch(std::exception& e) {
        std::cout << "ERROR::TEXTURE::LOAD_FAILED \n" << e.what() << std::endl;
    }
}