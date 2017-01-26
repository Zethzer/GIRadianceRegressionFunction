#version 330 core
out vec3 out_color;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 color1 = vec4(texture(texture1, TexCoords).rgba);
    vec4 color2 = vec4(texture(texture2, TexCoords).rgba);

    if(color2.w < color1.w)
      out_color = color1.xyz;
    else
      out_color = color2.xyz;
    
    out_color = color2.xyz;
}
