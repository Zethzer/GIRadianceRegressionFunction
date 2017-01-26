layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material
{
#ifdef TEXTURE
    sampler2D texture_diffuse1;
#else
    vec3 color;
#endif
#ifdef SPECULAR
    sampler2D texture_specular1;
#else
    vec3 specular;
#endif
#ifdef NORMAL
    sampler2D texture_normal1;
#endif
    float shininess;
    float roughness;
    float metalness;
    float refraction;
};

uniform Material material;

vec3 getDiffuse()
{
#ifdef TEXTURE
    return vec3(texture(material.texture_diffuse1, TexCoords));
#else
    return material.color;
#endif
}
vec3 getSpecular()
{
#ifdef SPECULAR
    return vec3(texture(material.texture_specular1, TexCoords));
#else
    return material.specular;
#endif
}
vec3 getNormal()
{
#ifdef NORMAL
    return normalize(vec3(texture(material.texture_normal1, TexCoords)) * 2.0 - 1.0);
#else
    return Normal;
#endif
}
float getRoughness()
{
    return material.roughness;
}
float getMetalness()
{
    return material.metalness;
}
float getRefraction()
{
    return material.refraction;
}


uniform vec3 viewPos;

const float NEAR = 1.f; // projection matrix's near plane
const float FAR = 100.0f; // projection matrix's far plane

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    gPositionDepth.xyz = FragPos;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gNormal = normalize(getNormal());
    gAlbedoSpec.rgb = getDiffuse();
    gAlbedoSpec.a = getSpecular().x;
    gMaterial = vec3(material.roughness, material.refraction, material.metalness);
}
