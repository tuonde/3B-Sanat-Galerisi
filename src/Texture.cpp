// ─────────────────────────────────────────────────────────────────────────────
// Texture.cpp  –  stb_image-based texture loader
// ─────────────────────────────────────────────────────────────────────────────

// stb_image is header-only; define the implementation exactly once
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Texture.h"
#include <iostream>

// ── Load ──────────────────────────────────────────────────────────────────────
unsigned int Texture::Load(const std::string& path, bool gammaCorrect)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // stb_image flips images vertically by default so that UV (0,0) is
    // bottom-left, matching OpenGL conventions.
    stbi_set_flip_vertically_on_load(true);

    int width, height, nChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);

    if(data)
    {
        GLenum internalFormat, dataFormat;

        // Choose format based on number of channels
        if(nChannels == 1)
        {
            internalFormat = GL_RED;
            dataFormat     = GL_RED;
        }
        else if(nChannels == 3)
        {
            internalFormat = gammaCorrect ? GL_SRGB  : GL_RGB;
            dataFormat     = GL_RGB;
        }
        else  // 4 channels (RGBA / PNG with alpha)
        {
            internalFormat = gammaCorrect ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat     = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,                 // mip level
                     internalFormat,
                     width, height,
                     0,                 // border (must be 0)
                     dataFormat,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // ── Wrapping: REPEAT is good for tiled textures ───────────────────────
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // ── Filtering: linear mip-map for smooth transitions ──────────────────
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "[Texture] Loaded: " << path
                  << "  (" << width << "x" << height
                  << ", ch=" << nChannels << ")\n";
    }
    else
    {
        std::cerr << "[Texture] ERROR: Failed to load: " << path << "\n"
                  << "  stb_image: " << stbi_failure_reason() << "\n";
        // Return a 1×1 magenta placeholder so rendering doesn't crash
        unsigned char magenta[3] = {255, 0, 255};
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, magenta);
    }

    stbi_image_free(data);
    return textureID;
}

// ── Bind ──────────────────────────────────────────────────────────────────────
void Texture::Bind(unsigned int id, unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}
