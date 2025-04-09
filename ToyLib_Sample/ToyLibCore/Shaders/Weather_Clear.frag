#version 410 core

out vec4 FragColor;
in vec3 vWorldDir;

uniform float uTime;

// --- ノイズ ---
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; ++i) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    vec2 cloudUV = vWorldDir.xz * 10.0 + uTime * vec2(0.05, 0.0);
    float cloudDensity = fbm(cloudUV); // 自然な雲形状

    float t = clamp(vWorldDir.y, 0.0, 1.0);
    //vec3 skyColor = mix(vec3(0.7, 0.8, 1.0), vec3(0.2, 0.3, 0.6), t); // 青空グラデ
    vec3 skyColor = mix(vec3(0.2, 0.3, 0.6), vec3(0.01, 0.03, 0.06), t); // 青空グラデ

    float cloudAlpha = smoothstep(0.3, 0.6, cloudDensity); // 雲らしいふんわり具合
    //vec3 finalColor = mix(skyColor, vec3(1.0), cloudAlpha * 0.8);     // 白雲を合成
    vec3 finalColor = mix(skyColor, vec3(1.0), cloudAlpha * 0.2);     // 白雲を合成

    FragColor = vec4(finalColor, 1.0);
}

/*
 #version 410 core

out vec4 FragColor;
in vec3 vWorldDir;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0-2.0*f);
    return mix(a, b, u.x) + (c - a)*u.y*(1.0 - u.x) + (d - b)*u.x*u.y;
}

void main()
{
    // XZ方向を強調して変化を出す
    vec2 uv = vWorldDir.xz * 100.0;
    float n = noise(uv); // fbm外してテスト

    FragColor = vec4(vec3(n), 1.0); // ← これで模様が出るかテスト！
}
*/
/*#version 410 core

out vec4 FragColor;
in vec3 vWorldDir;

uniform float uTime;

// --- 簡易ノイズ関数（ハッシュベース） ---
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; ++i) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    vec2 cloudUV = vWorldDir.xz * 10.0 + uTime * vec2(0.05, 0.0);
    float cloudDensity = fbm(cloudUV);

    float t = clamp(vWorldDir.y, 0.0, 1.0);
    vec3 skyColor = mix(vec3(0.7, 0.8, 1.0), vec3(0.2, 0.3, 0.6), t);

    float cloudAlpha = smoothstep(0.3, 0.6, cloudDensity); // ← 雲が出やすい
    vec3 finalColor = mix(skyColor, vec3(1.0), cloudAlpha * 0.8);

    FragColor = vec4(finalColor, 1.0);
}
*/
/* 雲
#version 410 core
in float vY;
out vec4 FragColor;

uniform float uTime;
uniform vec3 uSunDir;

// 2Dノイズ関数（簡易なFBM）
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    float t = clamp((vY + 1.0) * 0.5, 0.0, 1.0);
    vec3 baseSky = mix(vec3(0.8, 0.6, 0.3), vec3(0.1, 0.2, 0.5), t);

    // UV空間っぽい擬似値（Y方向と時間でアニメ）
    vec2 uv = vec2(gl_FragCoord.x / 800.0, gl_FragCoord.y / 600.0);
    uv += vec2(uTime * 0.01, 0.0); // 雲を流す

    float cloud = fbm(uv * 2.0);
    cloud = smoothstep(0.4, 0.7, cloud); // 雲の濃さ調整

    vec3 cloudColor = vec3(1.0); // 雲は白
    vec3 finalColor = mix(baseSky, cloudColor, cloud * 0.6); // α合成

    FragColor = vec4(finalColor, 1.0);
}
*/



/*
#version 410 core
in float vY;
out vec4 FragColor;

void main()
{
    float t = clamp((vY + 1.0) * 0.5, 0.0, 1.0); // Y -1.0〜1.0 → 0〜1
    vec3 topColor = vec3(0.1, 0.2, 0.5);         // 上空の色
    vec3 bottomColor = vec3(0.8, 0.6, 0.3);      // 地平の色
    vec3 color = mix(bottomColor, topColor, t); // 線形補間
    FragColor = vec4(color, 1.0);
}
*/
/*
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
*/
