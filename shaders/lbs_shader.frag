#version 410 core

float M_PI = 3.1415926535897932384626433832795;

in vec4 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;

float lightPow = 1.0;
float fd = 1.0/M_PI;
float alpha = 0.75;
vec4 albedo = vec4(0.5, 0.5, 0.5, 1.0);
vec4 lightPos = vec4(3.0, 3.0, 0.0, 1.0);

const float kd = 3.0;

void main(void)
{
    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
    vec4 wi = normalize(lightPos - gl_FragCoord);

    float diff = max(dot(normal, wi), 0.0);

    fragColor = vec4(0.15, 0.15, 0.15, 1.0)
            + kd * lightPow * fd * albedo * diff;
    fragColor.w = alpha;
}
