#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Mesh.h  –  GPU mesh: VAO/VBO/EBO wrapper
//
// A Mesh holds:
//   • a list of Vertex structs (position, normal, UV)
//   • a list of indices
//   • a list of Texture IDs + type names (used by the Assimp Model loader)
// ─────────────────────────────────────────────────────────────────────────────

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

// ── Data structures ──────────────────────────────────────────────────────────

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshTexture {
    unsigned int id;        // OpenGL texture ID
    std::string  type;      // "texture_diffuse1", "texture_specular1", …
    std::string  path;      // file path (used to avoid duplicate loads)
};

// ── Mesh class ───────────────────────────────────────────────────────────────

class Shader; // forward declaration

class Mesh
{
public:
    // ── Data ─────────────────────────────────────────────────────────────────
    std::vector<Vertex>      vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    // ── GPU handles ──────────────────────────────────────────────────────────
    unsigned int VAO, VBO, EBO;

    // ── Constructor/destructor ───────────────────────────────────────────────
    Mesh(const std::vector<Vertex>&       verts,
         const std::vector<unsigned int>& inds,
         const std::vector<MeshTexture>&  texs);
    ~Mesh();

    // Prevent accidental copies (GPU resources must be managed explicitly)
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow moves (needed for std::vector<Mesh>)
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Draw the mesh using the given shader
    void Draw(Shader& shader) const;

private:
    // Upload vertex/index data to the GPU
    void setupMesh();
};
