#version 410 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;

out float vY;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vY = aPosition.y; // 高さ（Y）を渡す
}
