#version 400 core

in vec4 fs_color;

out vec4 color;

void main(void)
{
    color = fs_color;
}