#version 410

// 頂点シェーダーからのデータ
// テクスチャ座標
in vec2 fragTexCoord;
// 法線ベクトル
in vec3 fragNormal;
// 座標
in vec3 fragWorldPos;

// アウトプットする色
out vec4 outColor;

// テクスチャ
uniform sampler2D uTexture;


// 色のUniform（この色がセットされていれば、強制的に塗りつぶし）
uniform vec3 uUniformColor; // 色を指定するUniform
// 強制的に塗りつぶす場合の条件
uniform bool uOverrideColor; // trueで色が強制的に適用される


// ディレクショナルライト
struct DirectionalLight
{
    vec3 mDirection;
    vec3 mDiffuseColor;
    vec3 mSpecColor;
};

// ライティングのUniforms
// カメラ位置
uniform vec3 uCameraPos;
// 反射光
uniform float uSpecPower;
// 環境光
uniform vec3 uAmbientLight;
// ディレクショナルライト
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

void main()
{
    // フォグの値を計算
    float dist = length(uCameraPos.xyz - fragWorldPos.xyz);
    float fogFactor = (uFoginfo.maxDist - dist) / (uFoginfo.maxDist - uFoginfo.minDist);
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    
    // 条件がtrueの場合、ライトやテクスチャを無視して色を強制的に塗りつぶし
    if (uOverrideColor)
    {
        // 強制的に塗りつぶす色をセット

        outColor = vec4(mix(uFoginfo.color, uUniformColor, fogFactor), 1.0f);
        return;
    }
    
    // 法線ベクトル
    vec3 N = normalize(fragNormal);
    // ライティング計算
    vec3 L = normalize(-uDirLight.mDirection);
    // 面からカメラのベクトル
    vec3 V = normalize(uCameraPos - fragWorldPos);
    // 反射角度
    vec3 R = normalize(reflect(-L, N));


    vec3 Phong = uAmbientLight;

    // 影の計算
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

    vec4 col = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);

    vec3 c = mix(uFoginfo.color, col.xyz, fogFactor);
    outColor = vec4(c, 1.0f);
    //outColor = col;//vec4(c, 1.0f);
}
