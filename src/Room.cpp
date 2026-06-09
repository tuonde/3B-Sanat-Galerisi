// ─────────────────────────────────────────────────────────────────────────────
// Room.cpp  –  Gallery room geometry (floor, ceiling, 4 walls, paintings, stand)
//
// Coordinate system
//   Y=0 : floor
//   Y=height : ceiling
//   X in [-width/2 … +width/2]
//   Z in [-depth/2 … +depth/2]   (+Z = "entrance" side, -Z = "back" wall)
// ─────────────────────────────────────────────────────────────────────────────

#include "Room.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Convenience alias
using namespace glm;

// ─────────────────────────────────────────────────────────────────────────────
// Helper: pack vertex data (pos3 + norm3 + uv2 = 8 floats) into a DrawObject
// ─────────────────────────────────────────────────────────────────────────────
DrawObject Room::buildQuad(const std::vector<float>& verts) const
{
    // Indices for two triangles (counter-clockwise winding)
    static const unsigned int idx[] = { 0,1,2, 2,3,0 };

    DrawObject obj;
    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(idx), idx, GL_STATIC_DRAW);

    // Stride = 8 floats × 4 bytes = 32 bytes
    GLsizei stride = 8 * sizeof(float);
    // Attribute 0: position (xyz)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    // Attribute 1: normal  (xyz)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
    // Attribute 2: UV      (uv)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(float)));

    glBindVertexArray(0);

    obj.indexCount = 6;
    return obj;
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper: unit cube centred at origin (6 faces × 4 verts, 36 indices)
// ─────────────────────────────────────────────────────────────────────────────
DrawObject Room::buildCube() const
{
    // 6 faces; each face = 4 vertices (pos3, norm3, uv2)
    // clang-format off
    static const float cubeVerts[] = {
        // ── Front face (+Z normal) ──────────────────────────────────────────
        -0.5f,-0.5f, 0.5f,  0,0,1,  0,0,
         0.5f,-0.5f, 0.5f,  0,0,1,  1,0,
         0.5f, 0.5f, 0.5f,  0,0,1,  1,1,
        -0.5f, 0.5f, 0.5f,  0,0,1,  0,1,
        // ── Back face (-Z normal) ───────────────────────────────────────────
         0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
        -0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
        -0.5f, 0.5f,-0.5f,  0,0,-1, 1,1,
         0.5f, 0.5f,-0.5f,  0,0,-1, 0,1,
        // ── Left face (-X normal) ───────────────────────────────────────────
        -0.5f,-0.5f,-0.5f, -1,0,0,  0,0,
        -0.5f,-0.5f, 0.5f, -1,0,0,  1,0,
        -0.5f, 0.5f, 0.5f, -1,0,0,  1,1,
        -0.5f, 0.5f,-0.5f, -1,0,0,  0,1,
        // ── Right face (+X normal) ──────────────────────────────────────────
         0.5f,-0.5f, 0.5f,  1,0,0,  0,0,
         0.5f,-0.5f,-0.5f,  1,0,0,  1,0,
         0.5f, 0.5f,-0.5f,  1,0,0,  1,1,
         0.5f, 0.5f, 0.5f,  1,0,0,  0,1,
        // ── Top face (+Y normal) ────────────────────────────────────────────
        -0.5f, 0.5f, 0.5f,  0,1,0,  0,0,
         0.5f, 0.5f, 0.5f,  0,1,0,  1,0,
         0.5f, 0.5f,-0.5f,  0,1,0,  1,1,
        -0.5f, 0.5f,-0.5f,  0,1,0,  0,1,
        // ── Bottom face (-Y normal) ─────────────────────────────────────────
        -0.5f,-0.5f,-0.5f,  0,-1,0, 0,0,
         0.5f,-0.5f,-0.5f,  0,-1,0, 1,0,
         0.5f,-0.5f, 0.5f,  0,-1,0, 1,1,
        -0.5f,-0.5f, 0.5f,  0,-1,0, 0,1,
    };
    // clang-format on

    // 6 faces × 2 triangles × 3 indices = 36
    static unsigned int cubeIdx[36];
    for(int f = 0; f < 6; ++f) {
        int base = f * 4;
        int i    = f * 6;
        cubeIdx[i+0] = base+0; cubeIdx[i+1] = base+1; cubeIdx[i+2] = base+2;
        cubeIdx[i+3] = base+2; cubeIdx[i+4] = base+3; cubeIdx[i+5] = base+0;
    }

    DrawObject obj;
    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(float)));

    glBindVertexArray(0);

    obj.indexCount = 36;
    return obj;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
Room::Room(float w, float h, float d)
    : width(w), height(h), depth(d)
{
    buildRoom();
    buildPaintings();
    buildStand();
}

// ─────────────────────────────────────────────────────────────────────────────
// Destructor
// ─────────────────────────────────────────────────────────────────────────────
Room::~Room()
{
    destroyObject(m_floor);
    destroyObject(m_ceiling);
    destroyObject(m_wallFront);
    destroyObject(m_wallBack);
    destroyObject(m_wallLeft);
    destroyObject(m_wallRight);
    destroyObject(m_paintRight);
    destroyObject(m_paintLeft);
    destroyObject(m_paintFront);
    destroyObject(m_stand);
}

// ─────────────────────────────────────────────────────────────────────────────
// Load Textures
// ─────────────────────────────────────────────────────────────────────────────
void Room::LoadTextures(const std::string& floorTex,
                        const std::string& wallTex,
                        const std::string& ceilingTex)
{
    m_floor.textureID   = Texture::Load(floorTex);
    m_floor.tiling      = 6.0f;  // repeat floor texture 6× for large rooms

    m_ceiling.textureID = Texture::Load(ceilingTex);
    m_ceiling.tiling    = 4.0f;

    unsigned int wallID = Texture::Load(wallTex);
    m_wallFront.textureID = m_wallBack.textureID =
    m_wallLeft.textureID  = m_wallRight.textureID = wallID;
    m_wallFront.tiling = m_wallBack.tiling = 2.0f;
    m_wallLeft.tiling  = m_wallRight.tiling = 3.0f;
}

void Room::LoadPaintings(const std::string& paintRight,
                         const std::string& paintLeft,
                         const std::string& paintFront)
{
    m_paintRight.textureID = Texture::Load(paintRight);
    m_paintLeft.textureID  = Texture::Load(paintLeft);
    m_paintFront.textureID = Texture::Load(paintFront);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw room surfaces
// ─────────────────────────────────────────────────────────────────────────────
static void drawObject(const DrawObject& obj, Shader& shader)
{
    Texture::Bind(obj.textureID, 0);
    shader.setInt  ("texture_diffuse1", 0);
    shader.setFloat("texTiling", obj.tiling);
    shader.setMat4 ("model", obj.model);
    shader.setBool ("hasTexture", obj.textureID != 0);

    glBindVertexArray(obj.VAO);
    glDrawElements(GL_TRIANGLES, obj.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Room::Draw(Shader& shader) const
{
    drawObject(m_floor,    shader);
    drawObject(m_ceiling,  shader);
    drawObject(m_wallFront,shader);
    drawObject(m_wallBack, shader);
    drawObject(m_wallLeft, shader);
    drawObject(m_wallRight,shader);
}

void Room::DrawPaintings(Shader& shader) const
{
    drawObject(m_paintRight, shader);
    drawObject(m_paintLeft,  shader);
    drawObject(m_paintFront, shader);
}

void Room::DrawStand(Shader& shader, unsigned int standTexID) const
{
    DrawObject s = m_stand;  // shallow copy – model matrix only
    s.textureID  = standTexID;
    s.tiling     = 1.0f;
    drawObject(s, shader);
}

// ─────────────────────────────────────────────────────────────────────────────
// Build room geometry
// ─────────────────────────────────────────────────────────────────────────────
void Room::buildRoom()
{
    float hw = width  * 0.5f;
    float hd = depth  * 0.5f;
    float H  = height;

    // ── Floor (Y=0, normal points up) ────────────────────────────────────────
    // pos3, norm3, uv2 – 4 vertices for a quad
    {
        std::vector<float> v = {
            -hw, 0, -hd,   0,1,0,  0,0,
             hw, 0, -hd,   0,1,0,  1,0,
             hw, 0,  hd,   0,1,0,  1,1,
            -hw, 0,  hd,   0,1,0,  0,1,
        };
        m_floor = buildQuad(v);
        m_floor.model = mat4(1.0f);
    }

    // ── Ceiling (Y=H, normal points down) ────────────────────────────────────
    {
        std::vector<float> v = {
            -hw, H, -hd,   0,-1,0,  0,0,
             hw, H, -hd,   0,-1,0,  1,0,
             hw, H,  hd,   0,-1,0,  1,1,
            -hw, H,  hd,   0,-1,0,  0,1,
        };
        m_ceiling = buildQuad(v);
        m_ceiling.model = mat4(1.0f);
    }

    // ── Back wall (Z=-hd, normal points +Z toward viewer) ────────────────────
    {
        std::vector<float> v = {
            -hw, 0, -hd,   0,0,1,  0,0,
             hw, 0, -hd,   0,0,1,  1,0,
             hw, H, -hd,   0,0,1,  1,1,
            -hw, H, -hd,   0,0,1,  0,1,
        };
        m_wallBack = buildQuad(v);
        m_wallBack.model = mat4(1.0f);
    }

    // ── Front wall (Z=+hd, normal points -Z toward viewer) ───────────────────
    {
        std::vector<float> v = {
             hw, 0, hd,   0,0,-1,  0,0,
            -hw, 0, hd,   0,0,-1,  1,0,
            -hw, H, hd,   0,0,-1,  1,1,
             hw, H, hd,   0,0,-1,  0,1,
        };
        m_wallFront = buildQuad(v);
        m_wallFront.model = mat4(1.0f);
    }

    // ── Left wall (X=-hw, normal points +X) ──────────────────────────────────
    {
        std::vector<float> v = {
            -hw, 0, -hd,   1,0,0,  0,0,
            -hw, 0,  hd,   1,0,0,  1,0,
            -hw, H,  hd,   1,0,0,  1,1,
            -hw, H, -hd,   1,0,0,  0,1,
        };
        m_wallLeft = buildQuad(v);
        m_wallLeft.model = mat4(1.0f);
    }

    // ── Right wall (X=+hw, normal points -X) ─────────────────────────────────
    {
        std::vector<float> v = {
             hw, 0,  hd,  -1,0,0,  0,0,
             hw, 0, -hd,  -1,0,0,  1,0,
             hw, H, -hd,  -1,0,0,  1,1,
             hw, H,  hd,  -1,0,0,  0,1,
        };
        m_wallRight = buildQuad(v);
        m_wallRight.model = mat4(1.0f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Build painting quads
// Each painting is a flat quad slightly in front of the wall surface
// (offset 0.02 units to avoid z-fighting)
// ─────────────────────────────────────────────────────────────────────────────
void Room::buildPaintings()
{
    float hw = width  * 0.5f;
    float hd = depth  * 0.5f;

    // ── Right wall painting (X=+hw, painting faces -X) ───────────────────────
    {
        float x   = hw - 0.02f;   // slightly off wall
        float cy  = height * 0.60f; // centre height
        float pw  = 2.5f;          // painting width
        float ph  = 1.8f;          // painting height

        std::vector<float> v = {
            x, cy-ph*0.5f, -pw*0.5f,  -1,0,0,  0,0,
            x, cy-ph*0.5f,  pw*0.5f,  -1,0,0,  1,0,
            x, cy+ph*0.5f,  pw*0.5f,  -1,0,0,  1,1,
            x, cy+ph*0.5f, -pw*0.5f,  -1,0,0,  0,1,
        };
        m_paintRight = buildQuad(v);
        m_paintRight.model  = mat4(1.0f);
        m_paintRight.tiling = 1.0f;
    }

    // ── Left wall painting (X=-hw, painting faces +X) ────────────────────────
    {
        float x   = -hw + 0.02f;
        float cy  = height * 0.60f;
        float pw  = 2.5f;
        float ph  = 1.8f;

        std::vector<float> v = {
            x, cy-ph*0.5f,  pw*0.5f,  1,0,0,  0,0,
            x, cy-ph*0.5f, -pw*0.5f,  1,0,0,  1,0,
            x, cy+ph*0.5f, -pw*0.5f,  1,0,0,  1,1,
            x, cy+ph*0.5f,  pw*0.5f,  1,0,0,  0,1,
        };
        m_paintLeft = buildQuad(v);
        m_paintLeft.model  = mat4(1.0f);
        m_paintLeft.tiling = 1.0f;
    }

    // ── Back wall painting (Z=-hd, painting faces +Z) ────────────────────────
    {
        float z   = -hd + 0.02f;
        float cy  = height * 0.60f;
        float pw  = 3.0f;
        float ph  = 2.0f;

        std::vector<float> v = {
            -pw*0.5f, cy-ph*0.5f, z,  0,0,1,  0,0,
             pw*0.5f, cy-ph*0.5f, z,  0,0,1,  1,0,
             pw*0.5f, cy+ph*0.5f, z,  0,0,1,  1,1,
            -pw*0.5f, cy+ph*0.5f, z,  0,0,1,  0,1,
        };
        m_paintFront = buildQuad(v);
        m_paintFront.model  = mat4(1.0f);
        m_paintFront.tiling = 1.0f;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Build stand (cube) – centred on X=0, Z=0
// ─────────────────────────────────────────────────────────────────────────────
void Room::buildStand()
{
    m_stand = buildCube();

    // Scale: 0.8 wide × 1.0 tall × 0.8 deep; translate so its base is on floor
    m_stand.model = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.0f))
                  * scale(mat4(1.0f), vec3(0.8f, 1.0f, 0.8f));
    m_stand.tiling = 1.0f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Free GPU resources for a single DrawObject
// ─────────────────────────────────────────────────────────────────────────────
void Room::destroyObject(DrawObject& obj)
{
    if(obj.VAO) { glDeleteVertexArrays(1, &obj.VAO); obj.VAO = 0; }
    if(obj.VBO) { glDeleteBuffers(1, &obj.VBO);      obj.VBO = 0; }
    if(obj.EBO) { glDeleteBuffers(1, &obj.EBO);      obj.EBO = 0; }
}
