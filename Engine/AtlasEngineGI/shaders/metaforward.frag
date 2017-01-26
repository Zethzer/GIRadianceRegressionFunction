layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out float Brightness;

uniform float M_PI = 3.1415926535897932384626433832795;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material
{
#ifdef TEXTURE
    sampler2D texture_diffuse1;
#else
    vec3 diffuse;
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
    return material.diffuse;
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


#ifdef POINTLIGHT
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    float intensity;
    vec3 color;
};
#endif



vec3 F_Schlick(vec3 f0, float f90, float u)
{
    return f0 + ( f90 - f0 ) * pow(1.f - u , 5.f);
}

float chiGGX(float v) {
    return v > 0.0f ? 1.0f : 0.0f;
}

float GGX_PartialGeometryTerm(vec3 v, vec3 n, vec3 h, float alpha)
{
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float chi = chiGGX(VdotH / clamp(dot(v, n),0.0,1.0));
    VdotH = VdotH * VdotH;
    
    float tan2 = (1-VdotH)/VdotH;
    return (chi * 2) / (1 + sqrt(1+alpha*alpha*tan2));
}


float D_GGX ( float NdotH , float m )
{
    float m2 = m * m ;
    float NdotH2 = NdotH * NdotH;
    float f = NdotH2 * m2 + (1 - NdotH2);
    return (chiGGX(NdotH) * m2) / (f * f * M_PI);
}


float Fr_DisneyDiffuse( float NdotV , float NdotL , float LdotH , float linearRoughness)
{
    float energyBias = linearRoughness * 0.5;
    float energyFactor = (1.0f - linearRoughness) * 1.0 + linearRoughness * (1.0 / 1.51);
    float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    vec3 f0 = vec3(1.0f);
    
    float lightScatter = F_Schlick(f0, fd90, NdotL).r;
    float viewScatter = F_Schlick( f0, fd90, NdotV).r;
    
    return lightScatter * viewScatter * energyFactor;
}

#ifdef POINTLIGHT
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 V, vec3 N, float NdotV, vec3 F0, float roughness, float linearRoughness, vec3 color)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);
    float LdotH = max(dot(L, H), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    
    //Specular
    vec3 F = F_Schlick(F0, 1, LdotH);
    float Vis = GGX_PartialGeometryTerm(V, N, H, roughness) * GGX_PartialGeometryTerm(L, N, H, roughness);
    float D = D_GGX(NdotH, roughness);
    vec3 Fr =  D * F * Vis / (4*NdotV*NdotH + 0.05);
    
    //Diffuse
    float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, linearRoughness);
    
    return (color / M_PI * Fd + Fr) * light.intensity * light.color * NdotL;
}
uniform PointLight pointLights[POINTLIGHT];
#endif

uniform vec3 viewPos;

void main()
{
    vec3 N = getNormal();
    vec3 V = normalize(viewPos - FragPos);
    
    float roughness = getRoughness();
    float linearRoughness = pow(roughness, 4);
    float ior = getRefraction();
    float metalness = getMetalness();
    vec3 color = getDiffuse();
    
    vec3 F0 = vec3(0.16 * ior * ior);
    F0 = (1.0f - metalness) * F0 + metalness * color;
    
    float NdotV = abs(dot(N , V)) + 1e-5f; // avoid artifact
    
    vec3 result = vec3(0, 0, 0);


#ifdef POINTLIGHT
    for(int i = 0; i < POINTLIGHT; i++)
        result += CalcPointLight(pointLights[i], FragPos, V, N, NdotV, F0, roughness, linearRoughness, color);
#endif
    
    Brightness = 0.0;
    float brightness = dot(result, vec3(0.2126f, 0.7152f, 0.0722f));
    
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    
    Brightness = brightness;

    FragColor = vec4(result, 1.0);
}
