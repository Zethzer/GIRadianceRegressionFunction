out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform vec2 window_size;

int kernelSize = 64;
float radius = 1.0;

const vec2 noiseSize = vec2(4.0, 4.0);

vec2 noiseScale = window_size / noiseSize;

uniform mat4 projection;

void main()
{
#ifdef SSAO
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 sample = TBN * samples[i];
        sample = fragPos + sample * radius;
        
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = -texture(gPositionDepth, offset.xy).w;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
#else
	FragColor = 1;
#endif
}
