#version 330 core

// ─── Vertex attributes ───────────────────────────────────────────────────────
layout (location = 0) in vec3 aPos;      // World-space position
layout (location = 1) in vec3 aNormal;   // Surface normal
layout (location = 2) in vec2 aTexCoord; // UV texture coordinate

// ─── Outputs to fragment shader ──────────────────────────────────────────────
out vec3 FragPos;    // Position in world space (for lighting calc)
out vec3 Normal;     // Normal in world space
out vec2 TexCoord;   // UV passed through

// ─── Uniform matrices ────────────────────────────────────────────────────────
uniform mat4 model;       // Object → World
uniform mat4 view;        // World  → Camera
uniform mat4 projection;  // Camera → Clip (perspective)

void main()
{
    // Transform position to world space (for lighting)
    FragPos  = vec3(model * vec4(aPos, 1.0));

    // Transform normal: use the inverse-transpose of model to handle non-uniform scale
    Normal   = mat3(transpose(inverse(model))) * aNormal;

    // Pass UV as-is
    TexCoord = aTexCoord;

    // Final clip-space position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
