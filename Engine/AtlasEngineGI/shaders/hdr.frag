layout (location = 0) out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float L_avg;

#ifdef CHROMATIC_ABERRATION
vec3 computeChromaticAberration()
{
    float rValue = texture(scene, vec2(TexCoords.x + 0.001, TexCoords.y)).r;
    float gValue = texture(scene, TexCoords).g;
    float bValue = texture(scene, vec2(TexCoords.x - 0.001, TexCoords.y)).b;
    return vec3(rValue, gValue, bValue);
}
#endif


void main()
{
    const float inv_gamma = 1.0/2.2;
    const float a = 0.18;
    
#ifdef CHROMATIC_ABERRATION
    vec3 hdrColor = computeChromaticAberration();
#else
    vec3 hdrColor = texture(scene, TexCoords).rgb;
#endif

#ifdef BLOOM
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;
#endif
    
    vec3 result;
    float new_exposure = 1;

#ifdef ADAPTATION
    new_exposure = 0.5/L_avg;
#endif
        
#ifdef HDR
    vec3 x = max(vec3(0), (hdrColor * new_exposure)-vec3(0.004));
    result = (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);
#else
    result = hdrColor;
#endif
    
    FragColor = result;
}

