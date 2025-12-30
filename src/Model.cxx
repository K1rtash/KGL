#include "Model.h"
#include "iostream"

Model::Model(const char* file) 
{
    loadModel(file);
}

void Model::Draw(Shader* shader, Camera* camera, Transform transform)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader, camera, &transform);
}  

Vertex processMeshVertex(aiMesh* mesh, unsigned int i)
{
    // Assimp llama vertices a la estructura Vertex (vec3 Pos, vec3 Normals, vec2 TexCoords)

    Vertex vertex; // El struct vertex que representa este ciclo

    glm::vec3 vec;
    vec.x = mesh->mVertices[i].x;
    vec.y = mesh->mVertices[i].y;
    vec.z = mesh->mVertices[i].z;
    vertex.Position = vec; // vec3 Pos

    vec.x = mesh->mNormals[i].x;
    vec.y = mesh->mNormals[i].y;
    vec.z = mesh->mNormals[i].z;
    vertex.Normal = vec; // vec3 Normal

    if(mesh->mTextureCoords[0]) // Tiene textura?
    {
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
    }
    else vertex.TexCoords = glm::vec2(0.0f, 0.0f);  

    return vertex; // Ya esta completa esta estructura de Vertex, la añadimos al vector que define este Mesh
}

vector<Texture> processMaterialTex(aiMaterial *mat, aiTextureType type, TextureType texType, std::string directory)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str; // nombre del archivo de la textura
        mat->GetTexture(type, i, &str);

        std::string texPath = directory + '/' + str.C_Str(); // ruta absoluta a la textura
        Texture texture{getDiscFileData(texPath.c_str()), i, texType}; // crea un objeto textura
        textures.push_back(texture); // lo añade al vector de tipo de texturas
    }
    return textures;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) // Vertices
        vertices.push_back(processMeshVertex(mesh, i));

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) 
    { // Indices
        aiFace face = mesh->mFaces[i]; // Assimp crea caras cada una con sus indices
        for(unsigned int j = 0; j < face.mNumIndices; j++) { 
            GLuint index = face.mIndices[j];
            indices.push_back(index);
        }
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = processMaterialTex(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE, directory); // vector de texturas de tipo diffuse
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); // añade las texturas diffuse al vector principal

        vector<Texture> specMaps = processMaterialTex(material, aiTextureType_SPECULAR, TextureType::SPECULAR, directory); // vector de texturas de tipo specular
        textures.insert(textures.end(), specMaps.begin(), specMaps.end()); // añade las texturas specular al vector principal
    }

    return Mesh{vertices, indices, textures};
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh processed_mesh = processMesh(mesh, scene);
        meshes.push_back(processed_mesh);			
    }
    // Funcion recursiva en todos los nodos hijos del nodo actual
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}