#version 330 core

// ─── Output ──────────────────────────────────────────────────────────────────
out vec4 FragColor;

// ─── Inputs from vertex shader ───────────────────────────────────────────────
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// ─── Textures ────────────────────────────────────────────────────────────────
uniform sampler2D texture_diffuse1;   // Primary diffuse texture

// ─── Lighting uniforms ───────────────────────────────────────────────────────
uniform vec3 lightPos;       // Point light position in world space
uniform vec3 lightColor;     // Light colour (usually white)
uniform vec3 viewPos;        // Camera / eye position for specular

// ─── Tiling ──────────────────────────────────────────────────────────────────
uniform float texTiling;     // Repeat factor (1.0 = no repeat)

void main()
{
    // ── Texture sample with tiling ───────────────────────────────────────────
    vec4 texColor = texture(texture_diffuse1, TexCoord * texTiling);

    // ── Ambient (base illumination so nothing is pitch-black) ────────────────
    float ambientStrength = 0.40;
    vec3  ambient = ambientStrength * lightColor;

    // ── Diffuse (Lambertian) ─────────────────────────────────────────────────
    vec3  norm      = normalize(Normal);
    vec3  lightDir  = normalize(lightPos - FragPos);
    float diff      = max(dot(norm, lightDir), 0.0);
    vec3  diffuse   = diff * lightColor;

    // ── Specular (Blinn-Phong, subtle) ───────────────────────────────────────
    float specularStrength = 0.15;
    vec3  viewDir   = normalize(viewPos - FragPos);
    vec3  halfwayDir = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3  specular  = specularStrength * spec * lightColor;

    // ── Combine ──────────────────────────────────────────────────────────────
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor   = vec4(result, texColor.a);
}
