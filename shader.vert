#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 viewMatrix;

void main(void)
{
    gl_Position = viewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
