// ─────────────────────────────────────────────────────────────────────────────
// Shader.cpp  –  GLSL shader program implementation
// ─────────────────────────────────────────────────────────────────────────────

#include "Shader.h"

// ── Constructor ───────────────────────────────────────────────────────────────
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. Read GLSL source from disk
    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);

    if(vertSrc.empty() || fragSrc.empty()) {
        std::cerr << "[Shader] ERROR: Could not read shader files:\n"
                  << "  vert: " << vertexPath << "\n"
                  << "  frag: " << fragmentPath << "\n";
        ID = 0;
        return;
    }

    // 2. Compile individual stages
    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    // 3. Link into a program
    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    checkLinkErrors(ID);

    // 4. Shader stages are no longer needed after linking
    glDeleteShader(vert);
    glDeleteShader(frag);
}

// ── Destructor ────────────────────────────────────────────────────────────────
Shader::~Shader()
{
    if(ID) glDeleteProgram(ID);
}

// ── Bind ─────────────────────────────────────────────────────────────────────
void Shader::use() const
{
    glUseProgram(ID);
}

// ── Uniform setters ───────────────────────────────────────────────────────────

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& m) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()),
                       1, GL_FALSE, &m[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
                       1, GL_FALSE, &m[0][0]);
}

// ── Private helpers ───────────────────────────────────────────────────────────

std::string Shader::readFile(const char* path)
{
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << "[Shader] Could not open: " << path << "\n";
        return {};
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compileShader(GLenum type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char*  src    = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int  success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[Shader] COMPILE ERROR ("
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "):\n" << infoLog << "\n";
    }
    return shader;
}

void Shader::checkLinkErrors(unsigned int program)
{
    int  success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[Shader] LINK ERROR:\n" << infoLog << "\n";
    }
}
