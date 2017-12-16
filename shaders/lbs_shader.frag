#version 410 core

float M_PI = 3.1415926535897932384626433832795;

in vec4 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;

float lightPow = 2.0;
float fd = 1.0/M_PI;
float alpha = 0.6;
vec4 albedo = vec4(0.5, 0.5, 0.5, 1.0);
vec4 lightPos = vec4(0.0, -20.0, 0.0, 1.0);

void main(void)
{
    vec4 wi = normalize(lightPos - gl_FragCoord/gl_FragCoord.w);

    fragColor = vec4(0.15, 0.15, 0.15, 1.0) + lightPow * fd * albedo * abs(dot(normal/normal.w, vec4(0.0, 1.0, 1.0, 1.0)));
    fragColor.w = alpha;
}
