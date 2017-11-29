#version 410 core

out vec4 fragColor;

in vec4 color;

void main(void)
{
    fragColor = color;
}
