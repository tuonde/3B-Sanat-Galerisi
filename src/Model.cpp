// ─────────────────────────────────────────────────────────────────────────────
// Model.cpp  –  Assimp-based model loader
// ─────────────────────────────────────────────────────────────────────────────

#include "Model.h"
#include "Shader.h"
#include "Texture.h"

#include <iostream>

// ── Constructor ───────────────────────────────────────────────────────────────
Model::Model(const std::string& path)
{
    Assimp::Importer importer;

    // aiProcess flags:
    //   Triangulate     – convert all polygons to triangles
    //   FlipUVs         – flip texture Y so UV (0,0) == bottom-left
    //   CalcTangentSpace– compute tangent/bitangent (useful for normal maps)
    //   GenNormals      – generate normals if the mesh has none
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate  |
        aiProcess_FlipUVs      |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals
    );

    if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cerr << "[Model] Assimp error: " << importer.GetErrorString() << "\n";
        return;
    }

    // The directory is needed to resolve relative texture paths
    m_directory = path.substr(0, path.find_last_of('/'));
    if(m_directory == path)
        m_directory = path.substr(0, path.find_last_of('\\'));

    processNode(scene->mRootNode, scene);
    m_loaded = true;

    std::cout << "[Model] Loaded: " << path
              << "  (" << m_meshes.size() << " meshes)\n";
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Model::Draw(Shader& shader) const
{
    for(const auto& mesh : m_meshes)
        mesh.Draw(shader);
}

// ── Private: traverse scene graph ────────────────────────────────────────────
void Model::processNode(aiNode* node, const aiScene* scene)
{
    // Process all meshes in this node
    for(unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    // Recursively process children
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

// ── Private: convert aiMesh → our Mesh ───────────────────────────────────────
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture>  textures;

    // ── Vertices ──────────────────────────────────────────────────────────────
    for(unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v;

        v.Position = glm::vec3(mesh->mVertices[i].x,
                               mesh->mVertices[i].y,
                               mesh->mVertices[i].z);

        v.Normal = mesh->HasNormals()
            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
            : glm::vec3(0.0f, 1.0f, 0.0f);

        // Only use the first UV channel (index 0)
        v.TexCoords = mesh->mTextureCoords[0]
            ? glm::vec2(mesh->mTextureCoords[0][i].x,
                        mesh->mTextureCoords[0][i].y)
            : glm::vec2(0.0f, 0.0f);

        vertices.push_back(v);
    }

    // ── Indices ───────────────────────────────────────────────────────────────
    for(unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // ── Material textures ─────────────────────────────────────────────────────
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        auto diffuseMaps = loadMaterialTextures(
            mat, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        auto specularMaps = loadMaterialTextures(
            mat, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

// ── Private: load textures from material ─────────────────────────────────────
std::vector<MeshTexture>
Model::loadMaterialTextures(aiMaterial* mat,
                             aiTextureType type,
                             const std::string& typeName)
{
    std::vector<MeshTexture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string texPath = m_directory + "/" + str.C_Str();

        // Check if already loaded (avoid duplicate GPU uploads)
        bool alreadyLoaded = false;
        for(auto& loaded : m_loadedTextures)
        {
            if(loaded.path == texPath)
            {
                textures.push_back(loaded);
                alreadyLoaded = true;
                break;
            }
        }

        if(!alreadyLoaded)
        {
            MeshTexture tex;
            tex.id   = Texture::Load(texPath);
            tex.type = typeName;
            tex.path = texPath;
            textures.push_back(tex);
            m_loadedTextures.push_back(tex);
        }
    }

    return textures;
}
