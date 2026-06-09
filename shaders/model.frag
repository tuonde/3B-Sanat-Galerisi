#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Assimp-loaded models may or may not have a texture.
// We provide a fallback colour when no texture is bound.
uniform sampler2D texture_diffuse1;
uniform bool      hasTexture;       // set to true when texture is present

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;           // fallback colour when hasTexture == false

uniform float texTiling;

void main()
{
    vec4 baseColor;
    if(hasTexture)
        baseColor = texture(texture_diffuse1, TexCoord * texTiling);
    else
        baseColor = vec4(objectColor, 1.0);

    // Ambient
    float ambientStrength = 0.40;
    vec3  ambient = ambientStrength * lightColor;

    // Diffuse
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(lightPos - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * lightColor;

    // Specular
    float specStr  = 0.25;
    vec3  viewDir  = normalize(viewPos - FragPos);
    vec3  halfDir  = normalize(lightDir + viewDir);
    float spec     = pow(max(dot(norm, halfDir), 0.0), 64.0);
    vec3  specular = specStr * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * baseColor.rgb;
    FragColor   = vec4(result, baseColor.a);
}
