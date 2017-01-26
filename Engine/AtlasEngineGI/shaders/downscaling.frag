#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform vec2 size;

void main()
{
    vec2 offset = 1/(2*size);
    FragColor = (texture(image, TexCoords).r
                +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y)).r
                +texture(image, vec2(TexCoords.x, TexCoords.y - offset.y)).r
                +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y - offset.y)).r
                +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y)).r
                +texture(image, vec2(TexCoords.x, TexCoords.y + offset.y)).r
                +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y + offset.y)).r
                +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y + offset.y)).r
                +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y - offset.y)).r)/9;
}
