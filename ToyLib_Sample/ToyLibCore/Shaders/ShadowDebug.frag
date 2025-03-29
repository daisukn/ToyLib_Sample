#version 410
in vec2 fragTexCoord;
out vec4 outColor;

uniform sampler2D uShadowMap;

void main()
{
    float depth = texture(uShadowMap, fragTexCoord).r;
    outColor = vec4(vec3(depth), 1.0); // 深度値をグレースケールで表示
}
