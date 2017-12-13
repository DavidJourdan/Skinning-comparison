#version 410 core

const uint MAX_BONE_COUNT = 8;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in float weights[8];
layout (location = 10) in uint indices[8];
layout (location = 18) in uint size;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 normal;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    normal = vec4(aNorm, 1.0);// projectionMatrix * viewMatrix * modelMatrix * vec4(aNorm, 1.0);
}
