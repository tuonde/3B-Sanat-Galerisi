// ─────────────────────────────────────────────────────────────────────────────
// Mesh.cpp  –  VAO/VBO/EBO mesh implementation
// ─────────────────────────────────────────────────────────────────────────────

#include "Mesh.h"
#include "Shader.h"
#include <utility>   // std::move, std::exchange

// ── Constructor ───────────────────────────────────────────────────────────────
Mesh::Mesh(const std::vector<Vertex>&       verts,
           const std::vector<unsigned int>& inds,
           const std::vector<MeshTexture>&  texs)
    : vertices(verts)
    , indices(inds)
    , textures(texs)
    , VAO(0), VBO(0), EBO(0)
{
    setupMesh();
}

// ── Destructor ────────────────────────────────────────────────────────────────
Mesh::~Mesh()
{
    if(VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    if(VBO) { glDeleteBuffers(1, &VBO);      VBO = 0; }
    if(EBO) { glDeleteBuffers(1, &EBO);      EBO = 0; }
}

// ── Move constructor ──────────────────────────────────────────────────────────
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices))
    , indices (std::move(other.indices))
    , textures(std::move(other.textures))
    , VAO(std::exchange(other.VAO, 0))
    , VBO(std::exchange(other.VBO, 0))
    , EBO(std::exchange(other.EBO, 0))
{}

// ── Move assignment ───────────────────────────────────────────────────────────
Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if(this != &other)
    {
        if(VAO) glDeleteVertexArrays(1, &VAO);
        if(VBO) glDeleteBuffers(1, &VBO);
        if(EBO) glDeleteBuffers(1, &EBO);

        vertices = std::move(other.vertices);
        indices  = std::move(other.indices);
        textures = std::move(other.textures);
        VAO = std::exchange(other.VAO, 0);
        VBO = std::exchange(other.VBO, 0);
        EBO = std::exchange(other.EBO, 0);
    }
    return *this;
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Mesh::Draw(Shader& shader) const
{
    // Bind textures: each type starts at unit 0, incrementing per type
    unsigned int diffuseN  = 1;
    unsigned int specularN = 1;

    for(unsigned int i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        // Build the uniform name, e.g. "texture_diffuse1"
        std::string name   = textures[i].type;
        std::string number;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseN++);
        else if(name == "texture_specular")
            number = std::to_string(specularN++);

        shader.setInt(name + number, (int)i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Draw
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset active texture to default
    glActiveTexture(GL_TEXTURE0);
}

// ── Private: GPU upload ───────────────────────────────────────────────────────
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // ── Upload vertices ───────────────────────────────────────────────────────
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // ── Upload indices ────────────────────────────────────────────────────────
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);

    // ── Vertex attributes ─────────────────────────────────────────────────────
    // Location 0: position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Position));

    // Location 1: normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));

    // Location 2: texture UV (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
