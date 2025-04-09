/*
#version 410 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 vWorldDir; // 雲ノイズに使う方向ベクトル

void main()
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldDir = normalize(worldPos.xyz); // 単位方向で出す
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
*/

#version 410 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 vWorldDir; // 雲ノイズに使う方向ベクトル
//out float vY;

void main()
{
    vec4 worldPos = vec4(aPosition, 1.0);// * uModel;
    vWorldDir = normalize(worldPos.xyz); // 単位方向で出す
    gl_Position = vec4(aPosition, 1.0) * uMVP;
    //vY = aPosition.y; // 高さ（Y）を渡す
}

