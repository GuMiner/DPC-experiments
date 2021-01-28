#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fs_normal;

uniform mat4 projectionMatrix;
uniform mat4 meshMatrix;

void main(void)
{
    vec4 vertexPos = meshMatrix * vec4(position, 1.0f);

    fs_normal = normalize(mat3(meshMatrix) * normal);
    gl_Position = projectionMatrix * vertexPos;
}
