#ifndef MODEL_CLASS
#define MODEL_CLASS

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	// Loads in a model from a file and stores tha information in 'data', 'JSON', and 'file'
	Model(const char* file);
 
	void Draw(Shader* shader, Camera* camera, Transform transform);

private:
    vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    //vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
#endif