#ifndef MODEL_CLASS
#define MODEL_CLASS

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
namespace fs = std::filesystem;

class Model
{
public:
	// Loads in a model from a file and stores tha information in 'data', 'JSON', and 'file'
	Model(fs::path file);
 
	void Draw(Shader* shader, Camera* camera, Transform transform);

private:
    vector<Mesh> meshes;
    vector<Texture> textures_loaded;
    fs::path source;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> processMaterialTex(aiMaterial *mat, aiTextureType type, Texture::Type texType, const aiScene* scene);
    std::string resolveTexturePath(const std::string& tex);
};
#endif