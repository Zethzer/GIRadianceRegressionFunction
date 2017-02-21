
//out vec4 FragColor;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec2 Brightness;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform samplerCube shadows;

uniform float far_plane;
uniform float near_plane;

uniform float M_PI = 3.1415926535897932384626433832795;

uniform vec3 viewPos;
const float gamma = 2.2;

#ifdef POINTLIGHT
struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    float intensity;
    vec3 color;
};
uniform PointLight pointLights[POINTLIGHT];
#endif

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;
  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = M_PI * denom * denom;
  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;
  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

#ifdef POINTLIGHT
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 V, vec3 N, vec3 F0, float roughness, float metalness, vec3 color)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);
    float distance = distance(light.position, fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;
  
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;
    float NdotL = max(dot(N, L), 0.0);
    
    vec3 fragToLight = fragPos - light.position;
    float closestDepth = texture(shadows, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.5;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return (kD * color / M_PI + brdf) * radiance * light.intensity * NdotL * (1-shadow);
}
#endif


void main()
{
  vec3 N = texture(gNormal, TexCoords).rgb;
  vec3 fragPos = vec3(texture(gPositionDepth, TexCoords).rgb);
  vec3 V = normalize(viewPos - fragPos);
  vec3 R = reflect(-V, N);
  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
  float roughness = texture(gMaterial, TexCoords).r;
  float metalness = texture(gMaterial, TexCoords).b;
  F0 = mix(F0, color, metalness);
  // reflectance equation
  vec3 lighting = vec3(0.0);
  
#ifdef POINTLIGHT
  for(int i = 0; i < POINTLIGHT; i++)
    lighting += CalcPointLight(pointLights[i], fragPos, V, N, F0, roughness, metalness, color);
#endif

    Brightness = vec2(0.0);
    float brightness = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    if(brightness > 1.0)
        BrightColor = vec4(lighting, 1.0);

    Brightness = vec2(brightness);

    FragColor = vec3(max(lighting, vec3(0)));
}
