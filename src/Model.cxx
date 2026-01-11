#include <iostream>
#include <filesystem>

#include <stb/stb_image.h>

#include "Model.h"
#include "Kirtash/stl.h"

Model::Model(fs::path file) : source{file}
{
    std::cout << "[INFO] Loading model: '" << file.string() << "'\n";
    loadModel(file.string());
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

/*std::string Model::resolveTexturePath(const std::string& tex)
{
    std::string modelPath = source.string();
    // 1. Normalizar barras
    std::string t = tex;
    for (char& c : t) if (c == '\\') c = '/';

    // 2. Si es textura embebida (*0, *1...)
    if (!t.empty() && t[0] == '*')
        return t;

    namespace fs = std::filesystem;

    fs::path modelDir = fs::path(modelPath).parent_path();
    fs::path texRel   = fs::path(t);
    fs::path texAbs   = modelDir / texRel;

    if (fs::exists(texAbs))
        return texAbs.string();

    // probar carpeta textures/
    fs::path alt = modelDir / "textures" / texRel.filename();
    if (fs::exists(alt))
        return alt.string();

    // probar al lado del glTF
    fs::path alt2 = modelDir / texRel.filename();
    if (fs::exists(alt2))
        return alt2.string();

    std::cout << "[ERROR] Texture for model '" << source.string() << "' not found!\n";
    return tex;
}*/

std::string Model::resolveTexturePath(const std::string& tex)
{
    if (tex.empty())
        return "";

    // Normalizar barras
    std::string t = tex;
    for (char& c : t) if (c == '\\') c = '/';

    // Textura embebida (*0, *1...)
    if (!t.empty() && t[0] == '*')
        return t;

    namespace fs = std::filesystem;
    fs::path modelDir = fs::path(source).parent_path(); // carpeta source/
    fs::path texPath = t; // ruta que viene de Assimp

    // Si es absoluta y existe, usarla
    if (texPath.is_absolute() && fs::exists(texPath))
        return texPath.string();

    // Si no existe relativa al modelo, ignorar path original y usar solo el nombre
    fs::path texCandidate = modelDir / texPath;
    if (!fs::exists(texCandidate))
        texPath = texPath.filename(); // solo nombre del archivo

    // Intentar carpeta textures/ al lado del modelo (layout estándar)
    fs::path texInTextures = modelDir.parent_path() / "textures" / texPath;
    if (fs::exists(texInTextures))
        return texInTextures.string();

    // Intentar al lado del modelo
    fs::path texBesideModel = modelDir / texPath;
    if (fs::exists(texBesideModel))
        return texBesideModel.string();

    // No encontrado
    std::cout << "[ERROR] Texture for model '" << source.string() << "' not found! (" << texPath.string() << ")\n";
    return tex;
}

Texture getEmbeddedAssimpTexture(const aiTexture* tex, Texture::Type type, unsigned int iterator)
{
    if (tex->mHeight == 0) {
        // Compressed
        unsigned char* data = reinterpret_cast<unsigned char*>(tex->pcData);
        int size = tex->mWidth;

        int w, h, ch;
        unsigned char* decoded = stbi_load_from_memory(data, size, &w, &h, &ch, 0);

        return Texture{decoded, w, h, ch, type, iterator};
    } 

    // RAW
    int w = tex->mWidth;
    int h = tex->mHeight;

    unsigned char* pixels = new unsigned char[w * h * 4];

    for (int i = 0; i < w * h; i++) {
        pixels[i*4 + 0] = tex->pcData[i].r;
        pixels[i*4 + 1] = tex->pcData[i].g;
        pixels[i*4 + 2] = tex->pcData[i].b;
        pixels[i*4 + 3] = tex->pcData[i].a;
    }

    return Texture{pixels, w, h, 4, type, iterator};
}


vector<Texture> Model::processMaterialTex(aiMaterial *mat, aiTextureType aiTexType, Texture::Type type, const aiScene* scene)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(aiTexType); i++)
    {
        aiString str; // nombre del archivo de la textura
        mat->GetTexture(aiTexType, i, &str);

        bool alreadyLoaded = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++) // itera todas las texturas cargadas comprobando si el path de cada una es igual que el actual
        {
            if( std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0 ) { // si coincide cargamos la actual y saltamos la carga normal
                textures.push_back(textures_loaded[j]); 
                alreadyLoaded = true;
                break;
            }
        }

        // Si la textura no esta ya en memoria, la cargamos y guardamos
        if(!alreadyLoaded)
        {            
            if (str.C_Str()[0] == '*') // textura embebida
            {
                const aiTexture* tex = scene->GetEmbeddedTexture(str.C_Str());
                Texture texture = getEmbeddedAssimpTexture(tex, type, i);

                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
            else // textura en disco
            {
                std::string texPath = resolveTexturePath(str.C_Str());
                Texture texture{texPath, type, i};
                            
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
    }
    return textures;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
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

        // Diffuse
        vector<Texture> diffuseMaps = processMaterialTex(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE, scene); // vector de texturas de tipo diffuse
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); // añade las texturas diffuse al vector principal

        // Specular
        vector<Texture> specMaps = processMaterialTex(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR, scene); // vector de texturas de tipo specular
        textures.insert(textures.end(), specMaps.begin(), specMaps.end()); // añade las texturas specular al vector principal

        // BaseColor
        vector<Texture> baseColorMaps = processMaterialTex(material, aiTextureType_BASE_COLOR, Texture::Type::BASE_COLOR, scene); 
        textures.insert(textures.end(), baseColorMaps.begin(), baseColorMaps.end());

        // Normal
        vector<Texture> normalMaps = processMaterialTex(material, aiTextureType_NORMALS, Texture::Type::NORMAL, scene); 
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // Height / Bump
        vector<Texture> heightMaps = processMaterialTex(material, aiTextureType_HEIGHT, Texture::Type::HEIGHT, scene);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // Metallic
        vector<Texture> metallicMaps = processMaterialTex(material, aiTextureType_METALNESS, Texture::Type::METALLIC, scene);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

        // Roughness
        vector<Texture> roughMaps = processMaterialTex(material, aiTextureType_DIFFUSE_ROUGHNESS, Texture::Type::ROUGHNESS, scene);
        textures.insert(textures.end(), roughMaps.begin(), roughMaps.end());

        // Metallic-Roughness (glTF usa esto)
        vector<Texture> mrMaps = processMaterialTex(material, aiTextureType_UNKNOWN, Texture::Type::METALLIC_ROUGHNESS, scene);
        textures.insert(textures.end(), mrMaps.begin(), mrMaps.end());

        // Ambient Occlusion
        vector<Texture> aoMaps = processMaterialTex(material, aiTextureType_AMBIENT_OCCLUSION, Texture::Type::AO, scene);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

        // Emissive
        vector<Texture> emissiveMaps = processMaterialTex(material, aiTextureType_EMISSIVE, Texture::Type::EMISSIVE, scene);
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    }

    return Mesh{vertices, indices, textures};
}

glm::mat4 processMat4(const aiMatrix4x4& m)
{
    glm::mat4 out;

    out[0][0] = m.a1; out[1][0] = m.a2; out[2][0] = m.a3; out[3][0] = m.a4;
    out[0][1] = m.b1; out[1][1] = m.b2; out[2][1] = m.b3; out[3][1] = m.b4;
    out[0][2] = m.c1; out[1][2] = m.c2; out[2][2] = m.c3; out[3][2] = m.c4;
    out[0][3] = m.d1; out[1][3] = m.d2; out[2][3] = m.d3; out[3][3] = m.d4;

    return out;
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
{
    glm::mat4 nodeTransform = processMat4(node->mTransformation);
    glm::mat4 globalTransform = parentTransform * nodeTransform;

    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh processed_mesh = processMesh(mesh, scene); // transforma el mesh de assimp al formato mio
        processed_mesh.transform = globalTransform; // añade la transformación del nodo

        meshes.push_back(processed_mesh);			
    }
    // Funcion recursiva en todos los nodos hijos del nodo actual
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, globalTransform);
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

    processNode(scene->mRootNode, scene, glm::mat4(1.0f)); // se procesa con una matrix inicial de identidad (1.0f)
}