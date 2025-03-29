#version 410

// 頂点シェーダーからのデータ
in vec4 fragPosLightSpace;  // ライト空間での位置

// 出力
out vec4 outColor;  // 深度のみ出力（カラー情報は不要）

// シャドウマップ（深度情報を格納）
uniform sampler2DShadow uShadowMap;

void main()
{
    // シャドウマップを生成するために、ライト空間の位置を深度に変換
    float depth = fragPosLightSpace.z / fragPosLightSpace.w;  // 正規化された深度

    // 深度マップの情報をシャドウマップに書き込む
    outColor = vec4(vec3(depth), 1.0);  // カラー情報は不要、深度だけを格納
}
