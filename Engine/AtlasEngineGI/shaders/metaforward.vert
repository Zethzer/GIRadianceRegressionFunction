layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
layout (location = 2) in vec2 texCoords;
#endif
#ifdef NORMAL
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
#endif

#if defined(ANIMATED) || defined(NORMAL)
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;
#endif

#if defined(ANIMATED) || !defined(NORMAL)
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
#endif

out vec3 FragPos;
out vec3 Normal;
#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
out vec2 TexCoords;
#endif

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#ifdef ANIMATED
const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];
#endif

void main()
{
#ifdef ANIMATED
    mat4 bone_transform = gBones[BoneIDs[0]] * Weights[0];
    bone_transform += gBones[BoneIDs[1]] * Weights[1];
    bone_transform += gBones[BoneIDs[2]] * Weights[2];
    bone_transform += gBones[BoneIDs[3]] * Weights[3];

    vec4 pos_L = bone_transform * vec4(position, 1.0);
    vec4 normal_L = bone_transform * vec4(normal, 0.0);
#else
    vec4 pos_L = vec4(position, 1.0);
    vec3 normal_L = transpose(inverse(mat3(model))) * normal;
#endif

    gl_Position = projection * view * model * pos_L;

    FragPos = vec4(pos_L).xyz;
    Normal = normal_L.xyz;

#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
    TexCoords = texCoords;
#endif
}
