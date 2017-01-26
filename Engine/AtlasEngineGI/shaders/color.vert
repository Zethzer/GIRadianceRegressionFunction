#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out float depth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  vec4 pos_L = vec4(position, 1.0);

  gl_Position = projection * view * model * pos_L;
}
