#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Shader.h  –  GLSL shader program wrapper
//
// Usage:
//   Shader sh("shaders/room.vert", "shaders/room.frag");
//   sh.use();
//   sh.setMat4("model", modelMatrix);
// ─────────────────────────────────────────────────────────────────────────────

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // OpenGL program ID
    unsigned int ID;

    // Constructor: reads and compiles both shader stages
    Shader(const char* vertexPath, const char* fragmentPath);

    // Destructor: frees GPU resources
    ~Shader();

    // Bind this program for rendering
    void use() const;

    // ── Uniform setters ──────────────────────────────────────────────────────
    void setBool (const std::string& name, bool  value)               const;
    void setInt  (const std::string& name, int   value)               const;
    void setFloat(const std::string& name, float value)               const;

    void setVec2 (const std::string& name, const glm::vec2& v)        const;
    void setVec3 (const std::string& name, const glm::vec3& v)        const;
    void setVec4 (const std::string& name, const glm::vec4& v)        const;

    void setMat3 (const std::string& name, const glm::mat3& m)        const;
    void setMat4 (const std::string& name, const glm::mat4& m)        const;

private:
    // Read a text file into a std::string
    static std::string readFile(const char* path);

    // Compile one shader stage; returns ID or 0 on error
    static unsigned int compileShader(GLenum type, const std::string& source);

    // Check link errors on the program
    static void checkLinkErrors(unsigned int program);
};
