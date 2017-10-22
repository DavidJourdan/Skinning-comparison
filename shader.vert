#version 450 core

// attribute vec4 qt_Vertex;
// attribute vec4 qt_MultiTexCoord0;
// uniform mat4 qt_ModelViewProjectionMatrix;
// varying vec4 qt_TexCoord0;

layout (location = 0) in vec3 aPos;

void main(void)
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
