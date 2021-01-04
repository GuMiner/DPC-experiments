#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 projectionMatrix;

out vec4 fs_color;

void main(void)
{
	fs_color = color;
	gl_PointSize = 5.0f;
    gl_Position = projectionMatrix * vec4(position, 1.0f);
}
