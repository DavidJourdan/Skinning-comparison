#version 410 core

const uint MAX_BONE_COUNT = 12;

const uint MAX_TRANSFORMATION_COUNT = 50;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec4 weights[MAX_BONE_COUNT / 4];
layout (location = 5) in uvec4 indices[MAX_BONE_COUNT / 4];
layout (location = 8) in uint size;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 tArr[MAX_TRANSFORMATION_COUNT];

out vec4 normal;

float getWeight(uint i) {
    uint q = i / 4;
    uint r = i % 4;
    return weights[q][r];
}

uint getIndex(uint i) {
    uint q = i / 4;
    uint r = i % 4;
    return indices[q][r];
}

void main(void)
{
    mat4 tMat = mat4(0.0);

    for (uint i = 0; i < size; ++i) {
        float w = getWeight(i);
        uint idx = getIndex(i);
        tMat += w * tArr[idx];
    }

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * tMat * vec4(aPos, 1.0);

    normal = vec4(aNorm, 1.0);
}
