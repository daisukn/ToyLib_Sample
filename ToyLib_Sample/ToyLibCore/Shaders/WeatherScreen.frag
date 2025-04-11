#version 410 core

out vec4 FragColor;

uniform float uTime;
uniform vec2 uResolution;
uniform float uRainAmount;   // 0.0〜1.0
uniform float uFogAmount;    // 0.0〜1.0
uniform float uSnowAmount;   // 0.0〜1.0

// --- 雨ノイズ ---
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(27.619, 57.583))) * 43758.5453);
}

float rainPattern(vec2 uv) {
    uv *= vec2(300.0, 1.0);
    uv.y += uTime * 8.0;
    float id = floor(uv.x);
    float offset = hash(vec2(id, 0.0));
    float y = fract(uv.y + offset);
    float brightness = smoothstep(0.0, 0.01, y) * (1.0 - y);
    return brightness;
}

// --- 雪ノイズ ---
float snowPattern(vec2 uv) {
    uv *= vec2(100.0, 1.0);
    uv.y += uTime * 0.5;
    float id = floor(uv);
    float offset = hash(id);
    float y = fract(uv.y + offset);
    float shape = smoothstep(0.98, 1.0, y) * (1.0 - y);
    return shape * 0.8;
}

// --- フォグ ---
float fogPattern(vec2 uv) {
    float dist = length(uv - vec2(0.5));
    float fog = smoothstep(0.3, 0.7, dist); // 周辺ほど濃く
    return 1.0 - fog;
}

void main()
{
    
    vec2 uv = gl_FragCoord.xy / uResolution;

    float rain = rainPattern(uv) * uRainAmount;
    float snow = snowPattern(uv) * uSnowAmount;
    float fog  = fogPattern(uv) * uFogAmount;

    float total = clamp(rain + snow, 0.0, 1.0);
    float alpha = total * 0.3 + fog * 0.5;

    vec3 color = mix(vec3(1.0), vec3(0.9, 0.95, 1.0), uSnowAmount); // 雪：少し青白く

    FragColor = vec4(color, alpha);
}

