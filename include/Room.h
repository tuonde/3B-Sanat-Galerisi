#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Room.h  –  Procedural gallery room geometry
//
// Generates six quads (floor, ceiling, 4 walls) plus painting planes and
// stand cubes, all with correct normals and UVs.
// ─────────────────────────────────────────────────────────────────────────────

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <string>

// ── Simple VAO bundle ────────────────────────────────────────────────────────
struct DrawObject {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int indexCount = 0;
    unsigned int textureID  = 0;
    glm::mat4    model      = glm::mat4(1.0f);
    float        tiling     = 1.0f;   // UV repeat factor
};

class Room
{
public:
    // ── Room dimensions (parametric) ─────────────────────────────────────────
    float width;    // X extent  (half-width on each side)
    float height;   // Y extent  (floor to ceiling)
    float depth;    // Z extent  (half-depth front to back)

    // ── Constructor: builds geometry and uploads to GPU ───────────────────────
    Room(float width  = 12.0f,
         float height =  6.0f,
         float depth  = 16.0f);

    ~Room();

    // Load textures from disk
    void LoadTextures(const std::string& floorTex,
                      const std::string& wallTex,
                      const std::string& ceilingTex);

    // Load painting textures (right wall, left wall, front wall)
    void LoadPaintings(const std::string& paintRight,
                       const std::string& paintLeft,
                       const std::string& paintFront);

    // Draw all room surfaces and stands (no model – that is drawn in main)
    void Draw(Shader& shader) const;

    // Draw only painting planes
    void DrawPaintings(Shader& shader) const;

    // Draw only the sculpture stand (cube in the centre)
    void DrawStand(Shader& shader, unsigned int standTexID) const;

private:
    // ── GPU objects ──────────────────────────────────────────────────────────
    DrawObject m_floor;
    DrawObject m_ceiling;
    DrawObject m_wallFront;  // -Z wall (where you enter)
    DrawObject m_wallBack;   // +Z wall (across from entrance)
    DrawObject m_wallLeft;   // -X wall
    DrawObject m_wallRight;  // +X wall

    // Paintings: one quad per wall
    DrawObject m_paintRight;
    DrawObject m_paintLeft;
    DrawObject m_paintFront;

    // Stand (cube)
    DrawObject m_stand;

    // ── Helpers ──────────────────────────────────────────────────────────────
    // Upload a quad (4 verts, 6 indices) and return a DrawObject
    DrawObject buildQuad(const std::vector<float>& verts) const;

    // Build a cube centred at origin with side = 1; scale via model matrix
    DrawObject buildCube() const;

    // Build all six room surfaces
    void buildRoom();

    // Build painting quads
    void buildPaintings();

    // Build the stand cube
    void buildStand();

    // Free a single DrawObject from GPU
    static void destroyObject(DrawObject& obj);
};
