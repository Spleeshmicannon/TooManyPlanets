#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 in_tex_coords;

out vec4 color;
out vec2 tex_coords;

uniform mat4 MVP;

void main()
{
	gl_Position = vec4(aPos, 0f, 1.0f) * MVP;
	color = vec4(aColor, 1.0);
	tex_coords = in_tex_coords;
}