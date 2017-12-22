#version 410 core

float M_PI = 3.1415926535897932384626433832795;

in vec4 normal;
in vec4 worldSpacePos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;

float lightPow = 1.0;
vec4 albedo = vec4(0.5, 0.5, 0.5, 1.0);

const uint lightCount = 2;
const vec4 lightPos[lightCount] = vec4[](vec4(3.0, 3.0, 0.0, 1.0), vec4(-3.0, -3.0, 0.0, 1.0));

const float ka = 0.15;
const float kd = 1.0;
const float ks = 1.0;
const float alpha = 2.0;

void main(void)
{
    fragColor = vec4(ka * vec3(1.0), 1.0);

    for (uint i = 0; i < lightCount; ++i) {
        vec4 wi = normalize(lightPos[i] - worldSpacePos);

        float diff = max(dot(normal, wi), 0.0);

        vec4 wo = wi - 2.0 * (wi - dot(wi, normal) * normal);
        vec4 v = normalize(inverse(projectionMatrix * viewMatrix) * vec4(0.0, 0.0, 1.0, 1.0));

        float spec = pow(max(dot(wo, v), 0.0), alpha);

        fragColor += (kd * diff + ks * spec) * lightPow * albedo;
    }

    fragColor.w = 1.0;
}
