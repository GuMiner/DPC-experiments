#version 400 core

out vec4 color;

in vec2 fs_uv;
in vec3 fs_normal;
in vec3 fs_view;
in vec3 fs_primaryPointLightVector;
in vec3 fs_secondaryPointLightVector;

uniform vec4 diffuseAlbedo; // Also contains transparency.
uniform vec4 specularAlbedo; // Also contains strength
uniform sampler2D geometryTexture;

// TODO actually use these and add in shading.
uniform vec3 directionalLightVector;
uniform vec3 directionalLightColor;
uniform float directionalLightStrength;

// Applies a standard sRGB 1/2.2 gamma correction.
vec4 applyGammaCorrection(vec4 color)
{
    return vec4(pow(color.xyz, vec3(0.4545f)), color.w);
}

void main(void)
{
    // Get our base color from the texture, apply lighting, and set the color
    vec4 baseColor = texture(geometryTexture, fs_uv);
    
    vec3 normal = normalize(fs_normal);
    vec3 primaryPL = normalize(fs_primaryPointLightVector);
    vec3 secondaryPL = normalize(fs_secondaryPointLightVector);
    vec3 view = normalize(fs_view);
    
    vec3 reflectPrimary = reflect(-primaryPL, normal);
    vec3 reflectSecondary = reflect(-secondaryPL, normal);
    
    vec3 diffuse = max(dot(normal, primaryPL), 0.0) * diffuseAlbedo.xyz + 
        max(dot(normal, secondaryPL), 0.0) * diffuseAlbedo.xyz;
    vec3 specular = pow(max(dot(reflectPrimary, view), 0.0), specularAlbedo.w) * specularAlbedo.xyz +
        pow(max(dot(reflectSecondary, view), 0.0), specularAlbedo.w) * specularAlbedo.xyz;
    
    vec4 effColor = vec4(baseColor.xyz * (diffuse + specular), baseColor.w * diffuseAlbedo.w);
    effColor = baseColor; // TODO: Remove. Makes our texture the only color. Used for debugging for now as we have no lights.
	color = applyGammaCorrection(effColor);
}