#version 410 core

in float vY;
out vec4 FragColor;

uniform float uTime;
uniform vec3 uSunDir;

void main()
{
    // グラデーション色（時間帯によって色味が変化）
    vec3 topColor = mix(vec3(0.2, 0.3, 0.6), vec3(0.05, 0.05, 0.1), 1.0 - uTime);     // 上空：青〜夜空
    vec3 bottomColor = mix(vec3(0.8, 0.6, 0.3), vec3(0.02, 0.02, 0.05), 1.0 - uTime); // 地平：橙〜暗

    float t = clamp((vY + 1.0) * 0.5, 0.0, 1.0);
    vec3 gradColor = mix(bottomColor, topColor, t);

    // 太陽ディスク（視線方向と太陽方向が近いほど強く光る）
    vec3 viewDir = normalize((gl_FragCoord.xyz - vec3(0.5, 0.5, 0.0))); // 疑似視線
    float sunIntensity = pow(max(dot(viewDir, normalize(uSunDir)), 0.0), 150.0);
    vec3 sunColor = vec3(1.0, 0.9, 0.6) * sunIntensity;

    FragColor = vec4(gradColor + sunColor, 1.0);
}
