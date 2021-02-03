#version 400 core

in vec4 fs_color;

out vec4 color;

void main(void)
{
    vec2 circleCoordinate = 2.0 * gl_PointCoord - 1.0;
    if (dot(circleCoordinate, circleCoordinate) > 1.0) {
        discard; // Not part of the circle / sphere, remove.
    }
    color = fs_color;
}