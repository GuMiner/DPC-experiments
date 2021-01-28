#version 400 core

out vec4 color;

in vec3 fs_normal;

void main(void)
{
    // Diffuse-only model lighting with 3 lights

    // If added together, these values should not be > vec3(1.0f)
    vec3 baseColor = vec3(0.5f, 0.5f, 0.4f);
    vec3 diffuseLightColor = vec3(0.4f, 0.4f, 0.5f);

    vec3 lightDirection = normalize(vec3(1.0f, 3.0f, 2.0f));
    vec3 lightDirection2 = normalize(vec3(2.0f, 1.0f, 3.0f));
    vec3 lightDirection3 = normalize(vec3(3.0f, 2.0f, 1.0f));
    vec3 normal = normalize(fs_normal);
    
    vec3 diffuse = abs(
        dot(normal, lightDirection) *
        dot(normal, lightDirection2) *
        dot(normal, lightDirection3) *
        3) * diffuseLightColor.xyz;
    
    color = vec4(baseColor.xyz + diffuse, 1.0f);
}