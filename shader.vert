#version 410 core

const uint MAX_BONE_COUNT = 12;

const uint MAX_TRANSFORMATION_COUNT = 50;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 cor;
layout (location = 3) in vec4 weights[MAX_BONE_COUNT / 4];
layout (location = 6) in uvec4 indices[MAX_BONE_COUNT / 4];
layout (location = 9) in uint size;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 tArr[MAX_TRANSFORMATION_COUNT];
uniform vec4 qArr[MAX_TRANSFORMATION_COUNT];

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

vec4 addQuat(vec4 p, vec4 q) {
    if(dot(p, q) >= 0)
        return p + q;
    else return p - q;
}

vec4 rotate(vec3 p, vec4 q) {
	vec4 p_q = vec4(dot(p, q.yzw), q.x*p - cross(p, q.yzw));
	return vec4(q.x*p_q.yzw + p_q.x*q.yzw + cross(q.yzw, p_q.yzw), 0.0);
}

void main(void)
{
    mat4 tMat = mat4(0.0);

    vec4 quat = vec4(0.0);

    for (uint i = 0; i < size; ++i) {
        float w = getWeight(i);
        uint idx = getIndex(i);
        tMat += w * tArr[idx];
        quat = addQuat(w * qArr[idx], quat);
    }

    quat = normalize(quat);
    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix; 
    gl_Position = MVP * (rotate(aPos - cor, quat) + tMat * vec4(cor, 1.0));

    normal = vec4(aNorm, 1.0);
}
