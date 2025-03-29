#version 410

// === 入力 ===
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

// フォグ情報
struct FogInfo {
    float maxDist;
    float minDist;
    vec3 color;
};
uniform FogInfo uFoginfo;

// シャドウマップ
uniform mat4 uLightSpaceMatrix;
uniform sampler2DShadow uShadowMap; // ← sampler2DShadow を使用！

void main()
{
    // === フォグの準備 ===
    float dist = length(uCameraPos - fragWorldPos);
    float fogFactor = (uFoginfo.maxDist - dist) / (uFoginfo.maxDist - uFoginfo.minDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

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
    if (NdotL > 0.0)
    {
        vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
        vec3 Specular = uDirLight.mSpecColor * pow(max(dot(R, V), 0.0), uSpecPower);
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
    //float shadow = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias),1);
    //float shadow = 1;
    /*float shadow = 1.0;
    if (projCoords.x >= 0.0 && projCoords.x <= 1.0 &&
        projCoords.y >= 0.0 && projCoords.y <= 1.0 &&
        projCoords.z >= 0.0 && projCoords.z <= 1.0)
    {
        shadow = textureProj(uShadowMap, vec4(projCoords.xy, projCoords.z - bias, 1.0));
        //float closestDepth = texture(uShadowMap, projCoords.xy).r;
        //float currentDepth = projCoords.z;
        
        //float bias = 0.005;
        //shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;
    }
    */
    if (shadow == 0)
    {
        //outColor = vec4(0, 1, 0, 1);
        //return;
    }
    shadow = mix(0.3, 1.0, shadow);
    // === テクスチャカラー計算 ===
    vec4 texColor = texture(uTexture, fragTexCoord);
    texColor.rgb *= Phong * shadow;

    // === フォグ適用 ===
    vec3 finalColor = mix(uFoginfo.color, texColor.rgb, fogFactor);
    outColor = vec4(finalColor, texColor.a);
    //outColor = texColor;
 
}
