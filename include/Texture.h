#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Texture.h  –  OpenGL texture loader (stb_image backend)
//
// Usage:
//   unsigned int id = Texture::Load("assets/textures/floor.jpg");
//   glBindTexture(GL_TEXTURE_2D, id);
// ─────────────────────────────────────────────────────────────────────────────

#include <glad/glad.h>
#include <string>

class Texture
{
public:
    // Load an image file and upload it to the GPU.
    // Returns the OpenGL texture ID, or 0 on failure.
    // gammaCorrect: if true, the internal format becomes GL_SRGB / GL_SRGB_ALPHA
    static unsigned int Load(const std::string& path,
                             bool gammaCorrect = false);

    // Bind a texture to a given texture unit (GL_TEXTURE0 + unit)
    static void Bind(unsigned int id, unsigned int unit = 0);
};
