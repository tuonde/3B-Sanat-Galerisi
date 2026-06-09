#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Model.h  –  Assimp-based 3D model loader
//
// Loads .obj (and other formats supported by Assimp) and stores the resulting
// Mesh objects for rendering.
// ─────────────────────────────────────────────────────────────────────────────

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

class Shader; // forward declaration

class Model
{
public:
    // Load a model from disk (path relative to the executable)
    explicit Model(const std::string& path);

    // Draw all meshes using the given shader
    void Draw(Shader& shader) const;

    // Whether the model was loaded successfully
    bool IsLoaded() const { return m_loaded; }

private:
    // ── Internal data ────────────────────────────────────────────────────────
    std::vector<Mesh>        m_meshes;
    std::string              m_directory;   // folder of the model file
    std::vector<MeshTexture> m_loadedTextures; // cache to avoid reloads
    bool                     m_loaded = false;

    // ── Assimp scene traversal ───────────────────────────────────────────────
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // ── Texture loading helper ───────────────────────────────────────────────
    std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat,
                                                  aiTextureType type,
                                                  const std::string& typeName);
};
