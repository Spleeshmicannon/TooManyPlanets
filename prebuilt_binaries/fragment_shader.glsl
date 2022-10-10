#version 330 core
out vec4 FragColor;
  
in vec4 color;
in vec2 tex_coords;

uniform sampler2D texture_data;

void main()
{
    FragColor = texture(texture_data, tex_coords) * color;
} 