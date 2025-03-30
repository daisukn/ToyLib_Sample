#version 410

// === 入力 ===
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

// 出力
out vec4 outColor;

// === Uniforms ===
uniform sampler2D uTexture;
uniform vec3 uUniformColor;
uniform bool uOverrideColor;
uniform vec3 uCameraPos;
uniform float uSpecPower;
uniform vec3 uAmbientLight;
uniform float uShadowBias;       // ← シャドウバイアスを外部から制御
uniform bool uUseToon;           // ← トゥーン有効/無効切り替え

// Directional Light
struct DirectionalLight {
    vec3 mDirection;
    vec3 mDiffuseColor;
    vec3 mSpecColor;
};
uniform DirectionalLight uDirLight;

// Fog
struct FogInfo {
    float maxDist;
    float minDist;
    vec3 color;
};
uniform FogInfo uFoginfo;

// Shadow Mapping
uniform mat4 uLightSpaceMatrix;
uniform sampler2DShadow uShadowMap;

// === 定数 ===
const float toonDiffuseThreshold = 0.5;
const float toonSpecThreshold = 0.95;

// === 関数：ライティング計算 ===
vec3 ComputeLighting(vec3 N, vec3 V, vec3 L)
{
    vec3 result = uAmbientLight;
    float NdotL = dot(N, L);

    if (NdotL > 0.0)
    {
        if (uUseToon)
        {
            float diffIntensity = step(toonDiffuseThreshold, NdotL);
            float specIntensity = pow(max(dot(reflect(-L, N), V), 0.0), uSpecPower);
            specIntensity = step(toonSpecThreshold, specIntensity);

            result += uDirLight.mDiffuseColor * diffIntensity;
            result += uDirLight.mSpecColor * specIntensity;
        }
        else
        {
            vec3 diffuse = uDirLight.mDiffuseColor * NdotL;
            vec3 specular = uDirLight.mSpecColor *
                pow(max(dot(reflect(-L, N), V), 0.0), uSpecPower);
            result += diffuse + specular;
        }
    }

    return result;
}

// === 関数：シャドウ判定 ===
float ComputeShadow(vec3 worldPos)
{
    vec4 lightSpacePos = vec4(worldPos, 1.0) * uLightSpaceMatrix;
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 1.0; // ライトが当たってる扱い
    }

    float shadow = textureProj(uShadowMap, vec4(projCoords.xy, projCoords.z - uShadowBias, 1.0));
    return mix(0.3, 1.0, shadow); // ソフトシャドウ効果
}

// === メイン ===
void main()
{
    // === フォグ計算 ===
    float dist = length(uCameraPos - fragWorldPos);
    float fogFactor = clamp((uFoginfo.maxDist - dist) / (uFoginfo.maxDist - uFoginfo.minDist), 0.0, 1.0);

    // === 強制カラーならそれを表示 ===
    if (uOverrideColor)
    {
        outColor = vec4(mix(uFoginfo.color, uUniformColor, fogFactor), 1.0);
        return;
    }

    // === ライティングとシャドウ ===
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(uCameraPos - fragWorldPos);
    vec3 L = normalize(-uDirLight.mDirection);
    
    vec3 lighting = ComputeLighting(N, V, L);
    float shadowFactor = ComputeShadow(fragWorldPos);

    // === テクスチャ色 × ライティング × シャドウ ===
    vec4 texColor = texture(uTexture, fragTexCoord);
    texColor.rgb *= lighting * shadowFactor;

    // === フォグ適用 ===
    vec3 finalColor = mix(uFoginfo.color, texColor.rgb, fogFactor);
    outColor = vec4(finalColor, texColor.a);
}



/*
// 頂点シェーダーから受け取るデータ
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

// 出力
out vec4 outColor;

// === Uniforms ===
// テクスチャ
uniform sampler2D uTexture;
// 強制塗りつぶしカラー
uniform vec3 uUniformColor;
uniform bool uOverrideColor;

// カメラ位置
uniform vec3 uCameraPos;

// マテリアル
uniform float uSpecPower;

// ライティング情報
uniform vec3 uAmbientLight;

struct DirectionalLight {
    vec3 mDirection;
    vec3 mDiffuseColor;
    vec3 mSpecColor;
};
uniform DirectionalLight uDirLight;

// フォグ関連
struct FogInfo {
    float maxDist;
    float minDist;
    vec3 color;
};
uniform FogInfo uFoginfo;

// トゥーンシェーディング用のパラメーター
const float toonThreshold = 0.5;
const float specThreshold = 0.95;

// シャドウマップ
uniform mat4 uLightSpaceMatrix;
uniform sampler2DShadow uShadowMap; // ← sampler2DShadow を使用！

void main()
{
    // フォグの値を計算
    float dist = length(uCameraPos.xyz - fragWorldPos.xyz);
    float fogFactor = (uFoginfo.maxDist - dist) / (uFoginfo.maxDist - uFoginfo.minDist);
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    
    // === 強制カラーがあれば即 return ===
    if (uOverrideColor)
    {
        vec3 overrideColor = mix(uFoginfo.color, uUniformColor, fogFactor);
        outColor = vec4(overrideColor, 1.0);
        return;
    }

    // === ライティング計算 ===
    vec3 N = normalize(fragNormal);                   // 法線
    vec3 L = normalize(-uDirLight.mDirection);        // ライト方向
    vec3 V = normalize(uCameraPos - fragWorldPos);    // 視線ベクトル
    vec3 R = reflect(-L, N);                          // 反射ベクトル

    vec3 Phong = uAmbientLight;
    float NdotL = dot(N, L);
    if (NdotL > 0)
    {

        vec3 Diffuse = uDirLight.mDiffuseColor;
        float intensity = step(toonThreshold, NdotL);
        Diffuse *= intensity;

        vec3 Specular = uDirLight.mSpecColor;
        intensity = pow(max(0.0, dot(R, V)), uSpecPower);
        intensity = step(specThreshold, intensity);
        Specular *= intensity;

        Phong += Diffuse + Specular;
    }

    // === シャドウ判定 ===
    // フラグメントのライト空間位置を0〜1に正規化
    //vec4 fragPosLightSpace = uLightSpaceMatrix * vec4(fragWorldPos, 1.0);
    vec4 fragPosLightSpace = vec4(fragWorldPos, 1.0) * uLightSpaceMatrix;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // [-1,1] → [0,1] にマップ
    float bias = 0.001;
    //float bias = max(0.005 * (1.0 - dot(N, L)), 0.001);
    float shadow = textureProj(uShadowMap, vec4(projCoords.xy, projCoords.z - bias, 1.0));
    shadow = mix(0.3, 1.0, shadow);
    // === テクスチャカラー計算 ===
    vec4 texColor = texture(uTexture, fragTexCoord);
    texColor.rgb *= Phong * shadow;

    // === フォグ適用 ===
    vec3 finalColor = mix(uFoginfo.color, texColor.rgb, fogFactor);
    outColor = vec4(finalColor, texColor.a);
}
*/
