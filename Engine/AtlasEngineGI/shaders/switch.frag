layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec2 Brightness;

in vec2 TexCoords;

uniform sampler2D directLighting;
uniform sampler2D indirectLighting;

void main()
{

	vec3 lighting = vec3(0.0);

#ifdef DIRECT_LIGHTING
	lighting = texture(directLighting, TexCoords).rgb;
#endif

#ifdef INDIRECT_LIGHTING
	lighting = texture(indirectLighting, TexCoords).rgb;
#endif

#ifdef COMPLETE_LIGHTING
	lighting = 0.5 * (texture(directLighting, TexCoords).rgb + texture(indirectLighting, TexCoords).rgb); 
#endif

	Brightness = vec2(0.0);
    float brightness = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    if(brightness > 1.0)
        BrightColor = vec4(lighting, 1.0);

    Brightness = vec2(brightness);

    FragColor = vec3(max(lighting, vec3(0)));
}