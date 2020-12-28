#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 fs_uv;
out vec3 fs_normal;
out vec3 fs_view;
out vec3 fs_primaryPointLightVector;
out vec3 fs_secondaryPointLightVector;

uniform vec3 primaryPointLightPosition;
uniform vec3 secondaryPointLightPosition;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    vec4 vertexPos = modelMatrix * vec4(position, 1.0f);
    
    fs_uv = uv;
    fs_normal = mat3(modelMatrix) * normal;
    fs_view = -vertexPos.xyz;
    fs_primaryPointLightVector = primaryPointLightPosition - vertexPos.xyz;
    fs_secondaryPointLightVector = secondaryPointLightPosition - vertexPos.xyz;
    
    gl_Position = projectionMatrix * vertexPos;
}
