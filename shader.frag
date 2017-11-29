#version 410 core

float M_PI = 3.1415926535897932384626433832795;

in vec4 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;

float lightPow = 2.0;
float fd = 1.0/M_PI;

vec4 albedo = vec4(0.5, 0.5, 0.5, 0.3);
vec4 lightPos = vec4(0.0, -20.0, 0.0, 1.0);

void main(void)
{
    vec4 wi = normalize(lightPos - gl_FragCoord);

    fragColor = 0.15 + lightPow * fd * albedo * abs(dot(normal, vec4(0.0, 0.0, 1.0, 0.0)));
}
