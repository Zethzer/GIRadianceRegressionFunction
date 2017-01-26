#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
    int level = 3;
    vec2 tex_offset = pow(2,level) / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb;
    
    for(int i = 1; i < 3; ++i)
    {
        result += texture(image, TexCoords + vec2(tex_offset.x * offset[i], 0.0)).rgb * weight[i];
        result += texture(image, TexCoords - vec2(tex_offset.x * offset[i], 0.0)).rgb * weight[i];
    }
    
    FragColor = vec4(result, 1.0);
}
