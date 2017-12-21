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

uniform vec4 dqTrNonDual[MAX_TRANSFORMATION_COUNT];
uniform vec4 dqTrDual[MAX_TRANSFORMATION_COUNT];

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
    uint s[MAX_TRANSFORMATION_COUNT];

    vec4 b0 = vec4(0.0);
    vec4 bEps = vec4(0.0);

    uint idx0 = getIndex(0);
    s[0] = 1;

    for (uint i = 1; i < size; ++i) {
        uint idx = getIndex(i);

        if(dot(dqTrNonDual[idx0], dqTrNonDual[idx]) >= 0)
            s[i] = 0;
        else
            s[i] = 1;
    }

    for (uint i = 0; i < size; ++i) {
        float w = getWeight(i);
        uint idx = getIndex(i);

        b0 += w * pow(-1, s[i]) * dqTrNonDual[idx];
        bEps += w * pow(-1, s[i]) * dqTrDual[idx];
    }

    float b0NormInv = 1.0/(length(b0));
    b0 = b0NormInv * b0;
    bEps = b0NormInv * bEps;

    float a0 = b0.w;
    vec3 d0 = b0.xyz;
    float aEps = bEps.w;
    vec3 dEps = bEps.xyz;

    vec3 vRes = aPos + 2.0 * cross(d0, cross(d0, aPos) + a0*aPos) + 2.0 *(a0*dEps - aEps*d0 + cross(d0, dEps));
    vec3 nRes = aNorm + 2.0 * cross(d0, cross(d0, aNorm) + a0*aNorm);

    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
    gl_Position = MVP * vec4(vRes, 1.0);

    normal = transpose(inverse(MVP)) * vec4(nRes, 1.0);
}
