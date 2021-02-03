#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform float pointScale;

out vec4 fs_color;

void main(void)
{
	float distanceToViewer = length(vec3(viewMatrix * vec4(position, 1.0f)));
	fs_color = color;
	gl_PointSize = pointScale / distanceToViewer;
    gl_Position = projectionMatrix * vec4(position, 1.0f);
}
